//
//  UUID.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/20.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import Foundation
import CoreBluetooth

let DeviceInfoServiceUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-052c2b448430");
let DeviceInfoServiceDeviceNameCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-052c2b448431");
let DeviceInfoServiceDeviceVersionCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-052c2b448432");
let DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-052c2b448433");

let DeviceInfoServiceDeviceDirectionCharacteristicUUID: CBUUID = CBUUID(string: "6f876815-73fa-49eb-9e5f-052c2b448434");


let EMSServiceUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448440");
let EMSServiceStatusCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448441");
let EMSServiceDriveCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448442");
let EMSServiceChannel1CharacteristicUUID: CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448443");
let EMSServiceChannel2CharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448444");
let EMSServiceChannel3CharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448445");
let EMSServiceChannel4CharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-152c2b448446");

let DeviceStatusServiceUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-252c2b448430");
let DeviceStatusServiceModeCharacteristicUUID : CBUUID = CBUUID(string:"6f876815-73fa-49eb-9e5f-252c2b448431");
