//
//  Packet.swift
//  CoachInTester
//
//  Created by Akira Matsuda on 2018/07/23.
//  Copyright © 2018 Akira Matsuda. All rights reserved.
//

import UIKit

protocol Packet {
    func byteArray() -> [UInt8]
}

class DrivePacket: Packet {
    var driveAll: Bool = false
    var channel: UInt8 = 0
    var delayMilliSeconds: UInt = 0
    
    init(channel: UInt8, delayMilliSeconds: UInt, driveAll: Bool = false) {
        self.channel = channel
        self.delayMilliSeconds = delayMilliSeconds
        self.driveAll = driveAll
    }
    
    func byteArray() -> [UInt8] {
        var firstByte: UInt8 = 0
        firstByte |= driveAll ? 1 : 0
        
        firstByte <<= 3
        firstByte |= (channel & 0b00000111)
        
        firstByte <<= 4
        firstByte |= (UInt8(delayMilliSeconds / 100) & 0b00001111)
        
        return [firstByte, 0]
    }
}

class ChannelPacket: Packet {
    private var channel: UInt8 = 0
    private var pulse: UInt = 40
    private var frequency: UInt = 50
    private var duration: UInt = 500
    
    init(channel: UInt8, pulse: UInt, frequency: UInt, duration: UInt) {
        self.channel = channel
        self.pulse = pulse
        self.frequency = frequency
        self.duration = duration
    }
    
    func byteArray() -> [UInt8] {
        var firstByte: UInt8 = 0
        firstByte |= (channel & 0b00000111)
        
        firstByte <<= 5
        var p = pulse
        p -= 40
        p /= 10
        firstByte |= (UInt8(p) & 0b00011111)
        
        var secondByte: UInt8 = 0
        var f = frequency
        f -= 50
        f /= 10
        secondByte |= (UInt8(f) & 0b00001111)
        
        firstByte <<= 4
        var d = duration
        d -= 500
        d /= 100
        secondByte |= (UInt8(d) & 0b00001111)
        
        return [firstByte, secondByte]
    }
}
