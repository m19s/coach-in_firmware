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

struct Row {
	let title: String
	var value: Data?
	let uuid: CBUUID
}

struct Section {
	let title: String
	let rows: [Row]
	let uuid: CBUUID
	
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
	
	var items: [Section] = []

	init(_ peripheral: CBPeripheral) {
		super.init(style: .grouped)
		self.peripheral = peripheral
		self.peripheral?.delegate = self
		
		self.items = [
			Section(title: "Device Info", rows: [
				Row(title: "Device name", value: nil, uuid: CBUUID(string: DeviceInfoServiceDeviceNameCharacteristicUUID)),
				Row(title: "Device version", value: nil, uuid: CBUUID(string: DeviceInfoServiceDeviceVersionCharacteristicUUID)),
				Row(title: "Firmware version", value: nil, uuid: CBUUID(string: DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID)),
				], uuid: CBUUID(string: DeviceInfoServiceUUID)),
			Section(title: "EMS Service", rows: [
				Row(title: "Channel 1", value: nil, uuid: CBUUID(string: EMSServiceChannel1CharacteristicUUID)),
				Row(title: "Channel 2", value: nil, uuid: CBUUID(string: EMSServiceChannel2CharacteristicUUID)),
				Row(title: "Channel 3", value: nil, uuid: CBUUID(string: EMSServiceChannel3CharacteristicUUID)),
				Row(title: "Channel 4", value: nil, uuid: CBUUID(string: EMSServiceChannel4CharacteristicUUID))
				], uuid: CBUUID(string: EMSServiceUUID)),
			Section(title: "Device mode", rows: [
				Row(title: "Mode", value: nil, uuid: CBUUID(string: DeviceInfoServiceDeviceNameCharacteristicUUID))
				], uuid: CBUUID(string: DeviceStatusServiceUUID))
		]
	}
	
	required init?(coder aDecoder: NSCoder) {
		fatalError("init(coder:) has not been implemented")
	}
	
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
		
		hud = JGProgressHUD()
		hud!.textLabel.text = "Loading..."
		hud!.show(in: UIApplication.shared.delegate!.window!!)
		peripheral!.discoverServices([CBUUID(string: DeviceInfoServiceUUID), CBUUID(string: EMSServiceUUID), CBUUID(string: DeviceStatusServiceUUID)])
    }
	
	override func viewDidAppear(_ animated: Bool) {
		super.viewDidAppear(animated)
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
		
		// Configure the cell...
		let section = items[indexPath.section]
		cell!.textLabel?.text = section.rows[indexPath.row].title
		if let data = section.rows[indexPath.row].value, let value = String(data: data, encoding: .utf8) {
			print(value)
			cell!.detailTextLabel?.text = value
		}
		else {
			cell!.detailTextLabel?.text = ""
		}
		
		return cell!
	}

	
	// CBPeripheralDelegate
	
	func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
		if error == nil, let services = peripheral.services {
			var uuids: [CBUUID] = []
			for s in services {
				switch s.uuid.uuidString {
				case CBUUID(string: DeviceInfoServiceUUID).uuidString :
					uuids.append(CBUUID(string: DeviceInfoServiceDeviceNameCharacteristicUUID))
					uuids.append(CBUUID(string: DeviceInfoServiceDeviceVersionCharacteristicUUID))
					uuids.append(CBUUID(string: DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID))
					break
				case CBUUID(string: EMSServiceUUID).uuidString :
					uuids.append(CBUUID(string: EMSServiceChannel1CharacteristicUUID))
					uuids.append(CBUUID(string: EMSServiceChannel2CharacteristicUUID))
					uuids.append(CBUUID(string: EMSServiceChannel3CharacteristicUUID))
					uuids.append(CBUUID(string: EMSServiceChannel4CharacteristicUUID))
					break
				case CBUUID(string: DeviceStatusServiceModeCharacteristicUUID).uuidString :
					uuids.append(CBUUID(string: DeviceStatusServiceModeCharacteristicUUID))
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
		case CBUUID(string: DeviceInfoServiceUUID).uuidString:
			section = items[0]
			break
		case CBUUID(string: EMSServiceUUID).uuidString:
			section = items[1]
			break
		case CBUUID(string: DeviceStatusServiceUUID).uuidString:
			section = items[2]
			break
		default:
			
			break
		}
		
		if let s = section, var row = s.search(uuid: characteristic.uuid) {
			row.value = characteristic.value
			tableView.reloadData()
		}
	}
}
