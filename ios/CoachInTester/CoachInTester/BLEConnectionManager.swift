//
//  BLEConnectionManager.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/23.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import UIKit
import CoreBluetooth

@objc protocol CBPeripheralDiscoverProtocol {
    var discoverImmediately: Bool { get set }
    var observers: [BLEPeripheralObserver] { get set }
}

private var DiscoverImmediatelyKey: UInt8 = 0
private var ObserversyKey: UInt8 = 0
extension CBPeripheral : CBPeripheralDiscoverProtocol {
    var discoverImmediately: Bool {
        get {
            guard let associatedObject = objc_getAssociatedObject(self, &DiscoverImmediatelyKey) as? NSNumber else {
                let associatedObject = NSNumber(value: Bool(false))
                objc_setAssociatedObject(self, &DiscoverImmediatelyKey, associatedObject, .OBJC_ASSOCIATION_RETAIN)
                return associatedObject.boolValue
            }
            return associatedObject.boolValue
        }
        
        set(discoverImmediately) {
            objc_setAssociatedObject(self, &DiscoverImmediatelyKey, NSNumber(value: discoverImmediately), .OBJC_ASSOCIATION_RETAIN)
        }
    }
    
    var observers: [BLEPeripheralObserver] {
        get {
            guard let associatedObject = objc_getAssociatedObject(self, &ObserversyKey) as? [BLEPeripheralObserver] else {
                let associatedObject: [BLEPeripheralObserver] = []
                objc_setAssociatedObject(self, &ObserversyKey, associatedObject, .OBJC_ASSOCIATION_RETAIN)
                return associatedObject
            }
            
            return associatedObject
        }
        
        set(peripheral) {
            objc_setAssociatedObject(self, &ObserversyKey, peripheral, .OBJC_ASSOCIATION_RETAIN)
        }
    }
}

class BLEPeripheralObserver: NSObject
{
    var didDiscoverService: ((_ peripheral: CBPeripheral, _ services: [CBService]?, _ error: Error?)->Void)?
    var didDiscoverCharacteristics: ((_ peripheral: CBPeripheral, _ service: CBService, _ error: Error?)->Void)?
    var didUpdateValue: ((_ peripheral: CBPeripheral, _ characteristic: CBCharacteristic, _ error: Error?)->Void)?
}

extension CBPeripheral {
    func characteristic(by: CBUUID) -> CBCharacteristic? {
        if let services = self.services {
            for s in services {
                if let characteristics = s.characteristics {
                    for c in characteristics {
                        if c.uuid.uuidString == by.uuidString {
                            return c
                        }
                    }
                }
            }
        }
        return nil
    }
}

extension BLEConnectionManager: CBPeripheralDelegate
{
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        for o in peripheral.observers {
            if let handler = o.didDiscoverService {
                handler(peripheral, peripheral.services, error)
            }
        }
        if error == nil, let services = peripheral.services {
            for s in services {
                peripheral.discoverCharacteristics(nil, for: s)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        for o in peripheral.observers {
            if let handler = o.didDiscoverCharacteristics {
                handler(peripheral, service, error)
            }
        }
        if error == nil, let characteristics = service.characteristics {
            for c in characteristics {
                peripheral.discoverDescriptors(for: c)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverDescriptorsFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        for o in peripheral.observers {
            if let handler = o.didUpdateValue {
                handler(peripheral, characteristic, error)
            }
        }
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor descriptor: CBDescriptor, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didWriteValueFor descriptor: CBDescriptor, error: Error?) {
        
    }
}

class BLEConnectionObserver: NSObject
{
    var onConnect: ((_ peripheral: CBPeripheral)->Void)?
    var onFailToConnect: ((_ peripheral: CBPeripheral)->Void)?
    var onDisconnect: ((_ peripheral: CBPeripheral)->Void)?
    var onDiscovered: ((_ peripheral: CBPeripheral, _ advertisementData: [String : Any], _ RSSI: NSNumber)->Void)?
}

class BLEConnectionManager: NSObject {
    static let shared = BLEConnectionManager()
    
    private var centralManager: CBCentralManager?
    var observers: [BLEConnectionObserver] = []
    
    override init() {
        super.init()
        
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func scan(withServices: [CBUUID]?, options: [String : Any]?, stopAfter: DispatchTime, completionHandler: @escaping (() -> Void)) {
        scan(withServices: withServices, options: options)
        DispatchQueue.main.asyncAfter(deadline: stopAfter) {
            self.centralManager?.stopScan()
            completionHandler()
        }
    }
    
    func scan(withServices: [CBUUID]?, options: [String : Any]?) {
        centralManager?.scanForPeripherals(withServices: withServices, options: options)
    }
    
    func connect(peripheral: CBPeripheral, options: [String : Any]?) {
        connect(peripheral: peripheral, options: options, discoverImmediately: true)
    }
    
    func connect(peripheral: CBPeripheral, options: [String : Any]?, discoverImmediately: Bool) {
        peripheral.discoverImmediately = discoverImmediately
        centralManager?.connect(peripheral, options: options)
    }
}

extension BLEConnectionManager: CBCentralManagerDelegate
{
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if peripheral.discoverImmediately {
            peripheral.delegate = self
            peripheral.discoverServices(nil)
        }
        
        for o in observers {
            if let handler = o.onConnect {
                handler(peripheral)
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        for o in observers {
            if let handler = o.onFailToConnect {
                handler(peripheral)
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        for o in observers {
            if let handler = o.onDiscovered {
                handler(peripheral, advertisementData, RSSI)
            }
        }
    }
}
