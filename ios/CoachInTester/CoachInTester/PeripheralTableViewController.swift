//
//  PeripheralTableViewController.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/18.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import UIKit
import CoreBluetooth
import JGProgressHUD

class PeripheralTableViewController: UITableViewController {

	var hud : JGProgressHUD?
	var centralManager: CBCentralManager!
	var peripherals: NSMutableSet = []

    override func viewDidLoad() {
        super.viewDidLoad()
		
		centralManager = CBCentralManager(delegate: self, queue: nil)

		let scanButton = UIBarButtonItem(title: "Scan", style: .plain, target: self, action: #selector(scan));
		navigationItem.rightBarButtonItem = scanButton
		
		tableView.register(UITableViewCell.self, forCellReuseIdentifier: "reuseIdentifier")
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return peripherals.count
    }
	
	@objc func scan() {
		peripherals = []
		centralManager.scanForPeripherals(withServices: nil, options: nil)
		
		hud = JGProgressHUD()
		hud!.textLabel.text = "Scanning..."
		hud!.show(in: UIApplication.shared.delegate!.window!!)
		hud!.dismiss(afterDelay: 5)
		DispatchQueue.main.asyncAfter(deadline: .now() + 5) {
			self.tableView.reloadData()
			self.centralManager.stopScan()
		}
	}

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "reuseIdentifier", for: indexPath)

        // Configure the cell...
		let peripheral = peripherals.allObjects[indexPath.row] as! CBPeripheral
		cell.textLabel?.text = peripheral.name
		
        return cell
    }
	
	override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
		let peripheral = peripherals.allObjects[indexPath.row] as! CBPeripheral
		if peripheral.state == .disconnected {
			centralManager.connect(peripheral, options: nil)
			hud = JGProgressHUD()
			hud!.textLabel.text = "Connecting..."
			hud!.show(in: UIApplication.shared.delegate!.window!!)
			tableView.deselectRow(at: indexPath, animated: true)
		}
		else {
			let viewController = OperationViewController(peripheral)
			navigationController?.pushViewController(viewController, animated: true)
		}
	}
}

extension PeripheralTableViewController: CBCentralManagerDelegate
{
	func centralManagerDidUpdateState(_ central: CBCentralManager) {
		
	}
	
	func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
		if let h = hud {
			h.indicatorView = JGProgressHUDSuccessIndicatorView()
			h.textLabel.text = "Connected"
			h.dismiss(afterDelay: 2)
			navigationController?.pushViewController(OperationViewController(peripheral), animated: true)
		}
	}
	
	func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
		if let h = hud {
			h.indicatorView = JGProgressHUDErrorIndicatorView()
			h.textLabel.text = "Connect failed"
			h.dismiss(afterDelay: 2)
		}
	}
	
	func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
		print(advertisementData)
		if let uuids = advertisementData[CBAdvertisementDataServiceUUIDsKey] as? Array<CBUUID> {
			for uuid in uuids {
				if uuid.uuidString == CBUUID(string: DeviceInfoServiceUUID).uuidString {
					peripherals.add(peripheral)
				}
			}
		}
	}
}
