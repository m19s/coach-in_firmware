//
//  OperationViewController.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/20.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import UIKit
import CoreBluetooth
import JGProgressHUD

class Row {
    init(title: String, value: Data?, uuid: CBUUID) {
        self.title = title
        self.value = value
        self.uuid = uuid
    }
    
	let title: String
	var value: Data?
	let uuid: CBUUID
}

class Section {
	let title: String
	var rows: [Row]
	let uuid: CBUUID
	
    init(title: String, rows: [Row], uuid: CBUUID) {
        self.title = title
        self.rows = rows
        self.uuid = uuid
    }
    
	func search(title: String) -> Row? {
		for r in rows {
			if r.title == title {
				return r
			}
		}
		
		return nil
	}
	
	func search(uuid: CBUUID) -> Row? {
		for r in rows {
			if r.uuid.uuidString == uuid.uuidString {
				return r
			}
		}
		
		return nil
	}
}

class OperationViewController: UITableViewController, CBPeripheralDelegate {
	
	private var peripheral: CBPeripheral?
	private var hud: JGProgressHUD?
    private var observer = BLEConnectionObserver()
    private var peripheralObserver = BLEPeripheralObserver()
    
    static let DeviceInfoSection = "Device Info"
    static let EMSServiceSection = "EMS Service"
    static let DeviceModeSection = "Device Mode"
    
	var items: [Section] = []

	init(_ peripheral: CBPeripheral) {
		super.init(style: .grouped)
		self.peripheral = peripheral
		
		self.items = [
            Section(title: OperationViewController.DeviceInfoSection, rows: [
				Row(title: "Device name", value: nil, uuid: DeviceInfoServiceDeviceNameCharacteristicUUID),
				Row(title: "Device version", value: nil, uuid: DeviceInfoServiceDeviceVersionCharacteristicUUID),
				Row(title: "Firmware version", value: nil, uuid: DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID),
				], uuid: DeviceInfoServiceUUID),
			Section(title: OperationViewController.EMSServiceSection, rows: [
                Row(title: "Drive", value: nil, uuid: EMSServiceDriveCharacteristicUUID),
				Row(title: "Channel 1", value: nil, uuid: EMSServiceChannel1CharacteristicUUID),
				Row(title: "Channel 2", value: nil, uuid: EMSServiceChannel2CharacteristicUUID),
				Row(title: "Channel 3", value: nil, uuid: EMSServiceChannel3CharacteristicUUID),
				Row(title: "Channel 4", value: nil, uuid: EMSServiceChannel4CharacteristicUUID)
				], uuid: EMSServiceUUID),
			Section(title: OperationViewController.DeviceModeSection, rows: [
				Row(title: "Mode", value: nil, uuid: DeviceInfoServiceDeviceNameCharacteristicUUID)
				], uuid: DeviceStatusServiceUUID)
		]
        
        peripheralObserver.didDiscoverCharacteristics = { [weak self] (peripheral, service, error) in
            guard let weakSelf = self else { return }
            if let h = weakSelf.hud {
                h.dismiss()
                if let characteristics = service.characteristics {
                    for c in characteristics {
                        peripheral.readValue(for: c)
                    }
                }
            }
        }
        
        peripheralObserver.didUpdateValue = { [weak self] (peripheral, characteristic, error) in
            guard let weakSelf = self else { return }
            if error == nil {
                for section in weakSelf.items {
                    let rows = section.rows.filter({ (row) -> Bool in
                        if row.uuid.uuidString == characteristic.uuid.uuidString {
                            return true
                        }
                        return false
                    })
                    for row in rows {
                        row.value = characteristic.value
                    }
                }
                weakSelf.tableView.reloadData()
            }
        }
        
        peripheral.observers.append(peripheralObserver)
        
        observer.onConnect = { [weak self] (peripheral) in
            guard let wealSelf = self else { return }
            if let h = wealSelf.hud {
                h.textLabel.text = "Discovering..."
            }
        }
        
        let manager = BLEConnectionManager.shared
        manager.observers.append(observer)
        
        if peripheral.state == .disconnected {
            BLEConnectionManager.shared.connect(peripheral: peripheral, options: nil)
            hud = JGProgressHUD()
            hud!.textLabel.text = "Connecting..."
            hud!.show(in: UIApplication.shared.delegate!.window!!)
        }
	}
	
	required init?(coder aDecoder: NSCoder) {
		fatalError("init(coder:) has not been implemented")
	}
    
    deinit {
        let manager = BLEConnectionManager.shared
        if let index = manager.observers.index(of: observer) {
            manager.observers.remove(at: index)
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        if let indexPath = tableView.indexPathForSelectedRow {
            tableView.deselectRow(at: indexPath, animated: true)
        }
    }

	override func numberOfSections(in tableView: UITableView) -> Int {
		return items.count
	}
	
	override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
		return items[section].title
	}
	
	override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return items[section].rows.count
	}
	
	override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
		let reuseIdentifier = "reuseIdentifier"
		var cell = tableView.dequeueReusableCell(withIdentifier: reuseIdentifier)
		if cell == nil {
			cell = UITableViewCell(style: .value1, reuseIdentifier: reuseIdentifier)
		}
		
        if indexPath.section == 0 {
            cell?.selectionStyle = .none
        }
        else {
            cell?.selectionStyle = .default
        }
        
		// Configure the cell...
		let section = items[indexPath.section]
		cell!.textLabel?.text = section.rows[indexPath.row].title
		if let data = section.rows[indexPath.row].value, let value = String(data: data, encoding: .utf8) {
			cell!.detailTextLabel?.text = value
		}
		else {
			cell!.detailTextLabel?.text = "-"
		}
		
		return cell!
	}
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
         tableView.deselectRow(at: indexPath, animated: true)
        let section = items[indexPath.section]
        
        switch section.title {
        case OperationViewController.EMSServiceSection: do {
            let row = section.rows[indexPath.row]
            if row.title == "Drive" {
                if let p = peripheral, let characteristic = p.characteristic(by: row.uuid) {
                    let packet = DrivePacket(channel: 0, delayMilliSeconds: 0, driveAll: true)
                    p.writeValue(Data(bytes: packet.byteArray()), for: characteristic, type: .withResponse)
                }
            }
            else {
                let viewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "ChannelViewController") as! ChannelViewController
                viewController.channelIdentifier = UInt8(indexPath.row - 1)
                viewController.peripheral = peripheral
                viewController.characteristicUUID = row.uuid
                navigationController?.pushViewController(viewController, animated: true)
            }
        }
            break
        default:

            break
        }
    }

	
	// CBPeripheralDelegate
	
	func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
		if error == nil, let services = peripheral.services {
			var uuids: [CBUUID] = []
			for s in services {
				switch s.uuid.uuidString {
				case DeviceInfoServiceUUID.uuidString :
					uuids.append(DeviceInfoServiceDeviceNameCharacteristicUUID)
					uuids.append(DeviceInfoServiceDeviceVersionCharacteristicUUID)
					uuids.append(DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID)
					break
				case EMSServiceUUID.uuidString :
					uuids.append(EMSServiceChannel1CharacteristicUUID)
					uuids.append(EMSServiceChannel2CharacteristicUUID)
					uuids.append(EMSServiceChannel3CharacteristicUUID)
					uuids.append(EMSServiceChannel4CharacteristicUUID)
					break
				case DeviceStatusServiceModeCharacteristicUUID.uuidString :
					uuids.append(DeviceStatusServiceModeCharacteristicUUID)
					break
				default:
					break
				}
				peripheral.discoverCharacteristics(uuids, for: s)
			}
		}
	}
	
	func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
		if let characteristics = service.characteristics {
			for c in characteristics {
				peripheral.readValue(for: c)
			}
		}
		
		if let h = hud {
			h.dismiss()
		}
	}
	
	func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
		var section: Section?
		switch characteristic.service.uuid.uuidString {
		case DeviceInfoServiceUUID.uuidString:
			section = items[0]
			break
		case EMSServiceUUID.uuidString:
			section = items[1]
			break
		case DeviceStatusServiceUUID.uuidString:
			section = items[2]
			break
		default:
			
			break
		}
		
        if let s = section, let row = s.search(uuid: characteristic.uuid) {
			row.value = characteristic.value
			tableView.reloadData()
		}
	}
}
