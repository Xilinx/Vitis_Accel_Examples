#!/usr/bin/python3
from x3ntg import *
from enum import Enum
import threading
import string
import time
import argparse
import datetime

from subprocess import Popen, PIPE
from pathlib import Path
from scapy.all import *

class Protocols(Enum):
    PRT_NONE = 0
    TCP = 1
    UDP = 2
    ARP = 3
    ICMP = 4
    PRT_ALL = 5

# test data, each sub-list contains :- [src_IP, src_Port, dst_IP, dst_Port, Protocol]
c_testIPPort = [["255.255.255.255", 53, "192.168.4.1",      53, Protocols.UDP], #0
                ["255.255.255.255", 53, "192.168.3.1",      53, Protocols.UDP], #1M
                ["255.255.255.255", 54, "192.168.3.1",      54, Protocols.UDP], #2
                ["255.255.255.255", 53, "128.0.0.23",       53, Protocols.TCP], #3M
                ["255.255.255.255", 53, "128.0.1.23",       53, Protocols.TCP], #4
                ["128.1.100.1",     87, "255.255.255.255",  87, Protocols.UDP], #5
                ["128.1.100.1",     87, "255.255.255.255",  87, Protocols.TCP], #6M
                ["191.168.8.1",     22, "255.255.255.255",  22, Protocols.TCP], #7
                ["191.168.4.2",     22, "255.255.255.255",  22, Protocols.TCP], #8
                ["191.168.4.1",     67, "255.255.255.255",  67, Protocols.TCP]] #9M
c_testResults = [0, 1, 0, 1, 0, 0, 1, 0, 0, 1]
c_expMatches = 4
c_bytes_per_frame = 152
# Delay between frames in nano seconds
c_interDelay = 2500
c_srcMac = ["06:05:05:05:05:06", "06:05:05:05:05:07", "06:05:05:05:05:08", "06:05:05:05:05:09", "06:05:05:05:05:0A",
            "06:05:05:05:05:0B", "06:05:05:05:05:0C", "06:05:05:05:05:0D", "06:05:05:05:05:0E", "06:05:05:05:05:0F"]
c_dstMac = ["06:06:04:09:08:07", "06:06:04:09:08:08", "06:06:04:09:08:09", "06:06:04:09:08:0A", "06:06:04:09:08:0B",
            "06:06:04:09:08:0C", "06:06:04:09:08:0D", "06:06:04:09:08:0E", "06:06:04:09:08:0F", "06:06:04:09:08:10"]
# ------------------------ Internal functions -------------------------------------
# create custom udp packet
def create_packet(srcMac, srcIpAddr, srcPort, dstMac, dstIpAddr, dstPort, protocol, payload):
    ether = Ether(src=srcMac, dst=dstMac, type=0x800)
    #hexdump(ether)
    ip = IP(dst=dstIpAddr, src=srcIpAddr)
    protHead = ""
    if protocol == Protocols.UDP:
        protHead = UDP(dport=dstPort, sport=srcPort)
    elif protocol == Protocols.TCP:
        protHead = TCP(dport=dstPort, sport=srcPort)
    pld = Raw(load=payload)
    pkt = ether / ip / protHead / pld
    #pkt = ip / protHead / pld
    return pkt

# Get 32-bit IP address from string representation of IP address.
def getIPFromString(ip_str):
    ip_num = 0
    ip_sub = ip_str.split('.')
    for i in range(len(ip_sub)):
        ip_num += (int(ip_sub[i]) << (i * 8))
    return ip_num

# Get String representation from 32-bit IP address.
def getIPString(ip):
    ip_str = ""
    for i in range(4):
        tmp = ((ip >> (i * 8)) & 15)
        ip_str += str(tmp)
        if i < 3:
            ip_str += "."
    return ip_str

# compare frames
def isFrameSame(packet, recvdBytes):
    ipVersion = (recvdBytes[14] >> 4) & 0x0F
    #print("Got:", ipVersion, "expected:", packet[IP].version)
    if (packet[IP].version == ipVersion):
        return True
    return False

# print frame header
def printFrameHeader(frame):
    for i in range(16):
        print(frame[i], end = ' ')
    print()

# read pcap and return the list of packets created from it
def readPcap(pcap_inp_file):
    packets = None
    #try:
    packets = rdpcap(pcap_inp_file)
    #except:
    #    print("Exception: Cannot read pcap file:", pcap_inp_file)
    return packets

def writePcap(pcap_file_path, packets):
    if packets is None or pcap_file_path is None:
        return False
    wrpcap(pcap_file_path, packets)
    return True

# prepate test packets for testing
def prepareTestData(write_pcap = False):
    payload = bytearray(c_bytes_per_frame)
    test_data = PacketList()
    for i in range(len(c_testIPPort)):
        c_packet = create_packet(c_srcMac[i], c_testIPPort[i][0], c_testIPPort[i][1], c_dstMac[i], c_testIPPort[i][2],
                                 c_testIPPort[i][3], c_testIPPort[i][4], payload)
        #hexdump(c_packet)
        test_data += PacketList([c_packet])
        #test_data.append(c_packet)
    print("Test Packets size:", len(test_data[0]), "bytes")
    if write_pcap:
        writePcap('testIn.pcap', test_data);
        print("Written test data in pcap file: testIn.pcap")
    return test_data

# pass the given list of packets to the given port
def passPackets(rxSendObj, x3_port_num, packets, verbose = True):
    frameCnt = len(packets)
    if verbose is True:
        print("Total frames to process -", frameCnt)
    for i in range(frameCnt):
        rxSendObj.enqueue(packets[i], x3_port_num, c_interDelay)
        print("Sent frame -", i+1, "of", len(packets), " size -", len(packets[i]), "bytes")
    rxSendObj.send()
    if verbose is True:
        print("Total frames sent: ", frameCnt)

# fetch the results
def getResults(rxRecieveObj, x3_port_num, packets, print_results = False):
    results = []
    # recieve same number of packets as sent
    total_frames = len(packets)
    rcv_cnt = 0
    while rcv_cnt < total_frames:
        frame = rxRecieveObj.receive(x3_port_num)
        if frame is not None:
            #frames_recvd.append(frame[0])
            cur_res = ""
            # match the header of the packets (first byte) to check the result
            results.append(int(isFrameSame(packets[rcv_cnt], frame[0])))
            # fetch message if to be printed
            if print_results is True:
                if results[-1] == 1:
                    cur_res = "ACCEPTED"
                else:
                    cur_res = "DROPPED"
            rcv_cnt += 1
            print("Got frame -", rcv_cnt, "of", total_frames, " size -", len(frame[0]), "bytes", cur_res)
    # recieved all expected frames
    print("Total frames received: ", rcv_cnt)
    return results

# validate the results
def validateResults(results, expected_results):
    success = True
    for i in range(len(expected_results)):
        if results[i] != expected_results[i]:
            success = False
            break
    return success

# read pcap, pass packets to x3 via ntg and print results
def processPcap(pcap_file, rxSendObj, rxRecieveObj, x3_port_num):
    packets = readPcap(pcap_file)
    if packets is not None:
        passPackets(rxSendObj, x3_port_num, packets)
        getResults(rxRecieveObj, x3_port_num, packets, print_results = True)
    else:
        print("Invalid PCAP file !!\nPlease try another one.")

# interactive console
def x3iConsole(x3_port, rx_in_obj, rx_out_obj):
    print("Enter full path of pcap to  process, press \'e\'/\'E\' to exit.")
    while True:
        cmd = input("$> ")
        if cmd == 'e' or cmd == 'E':
            break
        # if this file exists, process the pcap
        # else print error and move on
        if Path(cmd).is_file():
            processPcap(cmd, rx_in_obj, rx_out_obj, x3_port)
        else:
            print("Error: File does not exist!!\nPlease try another one.")
        #loop continue
    print("Exiting..")


# ------------------------ Top Level execution modules -------------------------------------
# run in interactive mode, accepting user pcaps in console
def runInteractiveFlow():
    # Targeting RX0 port
    x3_port = 0
    # Create RXIN class object
    rxSendObj = rxin()
    # Create RXOUT class object
    rxRecieveObj = rxout()
    print("Launching console..")
    x3iConsole(x3_port, rxSendObj, rxRecieveObj)

# run custom test flow for functional verification
def runTestFlow(testData):
    # Targeting RX0 port
    x3_port = 0
    # Create RXIN class object
    rxSendObj = rxin()
    # Create RXOUT class object
    rxRecieveObj = rxout()
    print("Waiting for emulation to start..")
    # wait for rule update to happen
    time.sleep(10)
    print("Sending packets: ", len(testData))
    # send packets
    passPackets(rxSendObj, x3_port, testData)
    # Receive Frames
    results = getResults(rxRecieveObj, x3_port, testData)
    # print and validate results
    print("Results...............")
    for i in range(len(results)):
        print(i, ".filter result:", results[i])
    # validate results
    success = validateResults(results, c_testResults)
    if success is True:
        print("SUCCESS: Results match. Test Passes !!")
    else:
        print("FAILURE: Incorrect matches. Test failed !!")
    print("----- NTG Test Done ----")
    
# Execute host and ntg in custom test flow
def runSCTest(host_exe, target, rule_file, test_packets):
    # run the system compiler host executable
    schost = Popen(["XCL_EMULATION_MODE=" + target + " ./" + host_exe + " -f 0 -r " + rule_file], shell=True)
    
    # Start a separate thread for sending input data and recieving output data
    ntg_io = threading.Thread(target=runTestFlow, args=(test_packets, ))
    ntg_io.start()
    
    # user interaction will be done by the host executable
    print("--------------------------------\n")
    print("******* Host exe Console ********\n")
    (host_output, err) = schost.communicate()
    # wait for schost to end
    print("Waiting for kernels to finish..\n")
    # print the message from host
    print("Enter any character to Exit !!\n> ", end = '')
    exit_code = schost.wait()
    print("---------- Host Done -----------\n")
    # Join thread to finish IO with kernel
    ntg_io.join()
    print("-------------Done---------------\n")

# ------------------------ Main function -------------------------------------
def runMain():
    # add arguments
    arg_p = argparse.ArgumentParser(description="Rx Passthrough Network Traffic Generation")
    arg_p.add_argument("-f", "--flow", required=False, help="Run test(0) or interactive flow (>= 1)")
    arg_p.add_argument("-p", "--pcap", required=False, help="pcap input file to enable pcap flow")
    # parse arguments
    args = arg_p.parse_args()
    run_flow = 0
    if args.flow is not None:
        run_flow = int(args.flow)
    if run_flow > 0:
        # interactvive flow
        runInteractiveFlow()
    else:
        # generate data
        test_packets = []
        if args.pcap == None:
            # generate data, i.e, packets with custom inputs for testing
            print("Test flow... preparing data.")
            test_packets = prepareTestData(True)
        else:
            pcap_file = Path(args.pcap)
            if pcap_file.is_file():
                test_packets = readPcap(args.pcap)
                if test_packets is None:
                    print ("Error: Invalid Pcap file !!")
                    return
            else:
                print("Error: Given pcap file does not exist!!\nPlease try another one.")
                return
        # execute tests
        # runSCTest(args.host_exe, args.target, args.rules_file, test_packets);
        runTestFlow(test_packets)

# ------------------------ Main script -------------------------------------
runMain()
print("Emulation Finished!")
