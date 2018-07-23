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

    var observer = BLEConnectionObserver()
	var hud : JGProgressHUD?
	var centralManager: CBCentralManager!
	var peripherals: NSMutableSet = []

    override func viewDidLoad() {
        super.viewDidLoad()
		
        observer.onDiscovered = { [weak self] (peripheral, advertisementData, rssi) in
            print(advertisementData)
            guard let weakSelf = self else { return }
            if let uuids = advertisementData[CBAdvertisementDataServiceUUIDsKey] as? Array<CBUUID> {
                for uuid in uuids {
                    if uuid.uuidString == CBUUID(string: DeviceInfoServiceUUID).uuidString {
                        weakSelf.peripherals.add(peripheral)
                        weakSelf.tableView.reloadData()
                        if let h = weakSelf.hud {
                            h.dismiss()
                            weakSelf.tableView.refreshControl?.endRefreshing()
                        }
                    }
                }
            }
        }
        
        observer.onFailToConnect = { [weak self] (peripheral) in
            guard let weakSelf = self else { return }
            if let h = weakSelf.hud {
                h.indicatorView = JGProgressHUDErrorIndicatorView()
                h.textLabel.text = "Connect failed"
                h.dismiss(afterDelay: 2)
            }
        }
        
        observer.onDisconnect = { [weak self] (peripheral) in
            guard let weakSelf = self else { return }
            weakSelf.navigationController?.popToRootViewController(animated: true)
        }
        
        let manager = BLEConnectionManager.shared
        manager.observers.append(observer)
        
		let scanButton = UIBarButtonItem(title: "Scan", style: .plain, target: self, action: #selector(scan));
		navigationItem.rightBarButtonItem = scanButton
		
		tableView.register(UITableViewCell.self, forCellReuseIdentifier: "reuseIdentifier")
        
        tableView.refreshControl = UIRefreshControl()
        tableView.refreshControl?.addTarget(self, action: #selector(scan), for: .valueChanged)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        DispatchQueue.main.asyncAfter(deadline: .now() + 1) {
            self.scan()
        }
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
        
        hud = JGProgressHUD()
        hud!.textLabel.text = "Scanning..."
        hud!.show(in: UIApplication.shared.delegate!.window!!)
        
        let manager = BLEConnectionManager.shared
        manager.scan(withServices: [CBUUID(string: DeviceInfoServiceUUID)], options: nil, stopAfter: .now() + 5) {
            if let h = self.hud {
                h.dismiss()
                self.tableView.reloadData()
                self.tableView.refreshControl?.endRefreshing()
            }
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
        let viewController = OperationViewController(peripheral)
        viewController.title = peripheral.name
        navigationController?.pushViewController(viewController, animated: true)
	}
}
