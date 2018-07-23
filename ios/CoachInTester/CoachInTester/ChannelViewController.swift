//
//  ChannelViewController.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/20.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import UIKit
import CoreBluetooth

class ChannelViewController: UIViewController {

    var peripheral: CBPeripheral?
    var characteristicUUID: CBUUID?
    
    @IBOutlet weak var durationLabel: UILabel!
    @IBOutlet weak var frequencyLabel: UILabel!
    @IBOutlet weak var pulseLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }
    
    @IBAction func sendChannelSettings(_ sender: Any) {
        if let p = peripheral, let uuid = characteristicUUID, let characteristic = p.characteristic(by: uuid) {
            let data = Data(bytes: [1, 1])
            p.writeValue(data, for: characteristic, type: .withResponse)
        }
    }

    @IBAction func durationStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        durationLabel.text = String(Int(value))
    }
    
    @IBAction func frequencyStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        frequencyLabel.text = String(Int(value))
    }
    
    
    @IBAction func pulseStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        pulseLabel.text = String(Int(value))
    }
}
