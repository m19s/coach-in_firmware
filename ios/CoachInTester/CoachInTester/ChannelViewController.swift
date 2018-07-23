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
    var channelIdentifier: UInt8 = 0
    var characteristicUUID: CBUUID?
    
    @IBOutlet weak var durationLabel: UILabel!
    @IBOutlet weak var frequencyLabel: UILabel!
    @IBOutlet weak var pulseLabel: UILabel!
    @IBOutlet weak var durationStepper: UIStepper!
    @IBOutlet weak var frequencyStepper: UIStepper!
    @IBOutlet weak var pulseStepper: UIStepper!
    @IBOutlet weak var bitStringLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        title = "Channel \(channelIdentifier + 1)"
        updateBitStringLabel()
    }
    
    @IBAction func sendChannelSettings(_ sender: Any) {
        if let p = peripheral, let uuid = characteristicUUID, let characteristic = p.characteristic(by: uuid) {
            let packet = ChannelPacket(channel: channelIdentifier, pulse: UInt(pulseStepper.value), frequency: UInt(frequencyStepper.value), duration: UInt(durationStepper.value))
            p.writeValue(Data(bytes: packet.byteArray()), for: characteristic, type: .withResponse)
        }
    }

    @IBAction func durationStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        durationLabel.text = String(Int(value))
        updateBitStringLabel()
    }
    
    @IBAction func frequencyStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        frequencyLabel.text = String(Int(value))
        updateBitStringLabel()
    }
    
    @IBAction func pulseStepperValueChanged(_ sender: UIStepper) {
        let value = sender.value
        pulseLabel.text = String(Int(value))
        updateBitStringLabel()
    }
    
    func updateBitStringLabel() {
        var message = ""
        
        let packet = ChannelPacket(channel: channelIdentifier, pulse: UInt(pulseStepper.value), frequency: UInt(frequencyStepper.value), duration: UInt(durationStepper.value))
        let firstByte = packet.byteArray()[0]
        message += String(firstByte, radix: 2)
        message += ", "
        let secondByte = packet.byteArray()[1]
        message += String(secondByte, radix: 2)
        
        bitStringLabel.text = message
    }
}
