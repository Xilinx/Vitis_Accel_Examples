from x3ntg import *
from general_defs import *
totalFrames = 10
flength = 102

payload = bytearray(flength)

# ether = Ether(src="06:05:05:05:05:05", dst="6:06:06:06:06:06")
# ip = IP(dst="192.68.0.1", src="192.168.1.1")
# udp = UDP(dport=100, sport=200)
# pld = Raw(load=payload)
# print(ether / ip / udp / pld)

dst_mac = "06:06:04:09:08:07"
src_mac = "06:05:05:05:05:05"
dst_ip = "192.168.0.1"
src_ip = "192.168.1.1"
dst_port = 0x1234
src_port = 0x4321

cmd = bytearray(4)
cmd[0] = 0  # NOP
cmd[1] = 4  # DROP
cmd[2] = 5  # MODIFY
cmd[3] = 6  # INSERT

# Create RX class object
rxSendObj = rxin()
rxRecieveObj = rxout()

# Create TX class object
txSendObj = txin()
txRecieveObj = txout()


def checkFrames(frame, length):
    if len(frame) < length:
        print("ERROR: LENGTH MISMATCH\n TESTCASE FAILED")
        return 1
    f_detail = Ether(bytes(frame))
    if f_detail[Ether].dst != dst_mac or f_detail[Ether].src != src_mac:
        print("ERROR: MAC MISMATCH\n TESTCASE FAILED")
        return 1
    if f_detail[IP].dst != dst_ip or f_detail[IP].src != src_ip:
        print("ERROR: IP MISMATCH\n TESTCASE FAILED")
        return 1
    if f_detail[UDP].dport != dst_port or f_detail[UDP].sport != src_port:
        print("ERROR: UDP MISMATCH\n TESTCASE FAILED")
        return 1
    return 0


for packets in range(totalFrames):
    payload[0] = cmd[packets % 4]
    payload[1] = packets
    pkt = create_udp_packet(dstMacAddr=dst_mac, dstIpAddr=dst_ip,
                            dstUdpPort=dst_port, payload=payload,
                            srcMacAddr=src_mac, srcIpAddr=src_ip,
                            srcUdpPort=src_port)
    opType="NOP"
    if payload[0] == 4:
        opType="DROP"
    elif payload[0] == 5:
        opType="MODIFY"
    elif payload[0] == 6:
        opType="INSERT"
    print("sending udp pkt length", len(pkt), "dst_port", dst_port, "cmd",opType)
    
    
    txSendObj.enqueue(pkt, 0, 1500)

txSendObj.send()

frame_id = 0
frames = []
while frame_id < 10:
    # [X3NTG API]: Recieve Frames List using RXOUT Object
    frames_list = txRecieveObj.receive(0)

    if frames_list is None:
        continue
    else:
        # Read frames from list and
        frame_id += 1
        rxSendObj.enqueue(frames_list[0], 0, 1500)

rxSendObj.send()

frame_id = 0

while frame_id < 7:
    # [X3NTG API]: Recieve Frames List using RXOUT Object
    frames_list = rxRecieveObj.receive(0)

    if frames_list is None:
        continue
    else:
        # Read frames from list and
        frame_id += 1
        frames.append(frames_list[0])

fid = 0
frameNum = 0
error = 0
print("Number of Frames received ", len(frames))
while frameNum < totalFrames:
    expected_cmd = cmd[frameNum % 4]
    if expected_cmd == 0:
        # NOP
        error += checkFrames(frames[fid], flength + 42)
        fid += 1
    elif expected_cmd == 5:
        # MODIFY
        error += checkFrames(frames[fid], flength + 42)
        fid += 1
    elif expected_cmd == 6:
        # INSERT
        error += checkFrames(frames[fid], flength + 44)
        fid += 1
    frameNum += 1

if error == 0:
    print("Frame Based NTG API Flow Ran successfully")
    print("TEST PASSED")
