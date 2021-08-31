#!/usr/bin/env python3
import os
import sys
import subprocess
import csv
import xml.etree.ElementTree as ET
import json
import time
import re
import argparse
from argparse import ArgumentParser

##########################################################
# Configuration
failure_msgs_ON_HLS_WARNING = True




##########################################################



XOCC_KRNL_FREQ = "--kernel_frequency"



HLS_XML_REPORT = "csynth.xml"
HLS_LOG_FILE = "vitis_hls.log"

QOR_FILENAME = 'qor.json'
QOR_CHECK = 'qor_check'
XCLBINS = 'containers'
KERNELS = 'accelerators'
CHK_SYS_TIMING = 'meet_system_timing'
CHK_TIMING = 'check_timing'
CHK_WARNING = 'check_warning'
CHK_LATENCY = 'check_latency'
LOOPS = 'loops'

PROFILE = 'profile'
CHK_PIPETYPE = 'PipelineType'
PIPE_II = 'PipelineII'
PIPE_DEPTH = 'PipelineDepth'

VER = 'ReportVersion'
USER_ASSIGNMENTS = 'UserAssignments'
TARGET_CLK_PERIOD = 'TargetClockPeriod'
CLK_UNCERTAINTY = 'ClockUncertainty'
UNIT = 'unit'
PERF_EST = 'PerformanceEstimates'
SUM_TIMING = 'SummaryOfTimingAnalysis'
SUM_OVERALL_LAT = 'SummaryOfOverallLatency'
SUM_LOOP_LAT = 'SummaryOfLoopLatency'
EST_CLK = 'EstimatedClockPeriod'

AREA_EST = 'AreaEstimates'
RESOURCES = 'Resources'
AVAIL_RES = 'AvailableResources'

SUM_IF = 'InterfaceSummary'
RTL_PORTS = 'RtlPorts'

NAME = 'name'
ERROR = 'WARNING'

def is_json(myjson_string):
    try:
        json_object = json.loads(myjson_string)
    except ValueError as e:
        return False, e
    return True, ''


def find_key(dic, key, find_multi=False):

    if dic is False or dic is None:
        return [], None

    stack = list(dic.items())
    visited = set()
    ret = []
    while stack:
        k, v = stack.pop()
        if isinstance(v, dict):
            if k not in visited:
                stack.extend(v.items())
                if key == k:
                    # print("%s: %s" % (k, v))
                    if find_multi is False:
                        return k, v
                    else:
                        ret.append({k: v})
            # else:
            #    print("%s: %s" % (k, v))
        visited.add(k)

    return ret, None



def list_keys_dic(dic):
    for key, value in dic.iteritems():
        print("Key: " + str(key))


# import xml.etree.ElementTree
# make_dict_from_tree(xml.etree.ElementTree.fromstring(xml_string))
def make_dict_from_tree(element_tree):
    """Traverse the given XML element tree to convert it into a dictionary.

    :param element_tree: An XML element tree
    :type element_tree: xml.etree.ElementTree
    :rtype: dict
    """

    def internal_iter(tree, accum):
        """Recursively iterate through the elements of the tree accumulating
        a dictionary result.

        :param tree: The XML element tree
        :type tree: xml.etree.ElementTree
        :param accum: Dictionary into which data is accumulated
        :type accum: dict
        :rtype: dict
        """
        if tree is None:
            return accum

        if tree.getchildren():
            accum[tree.tag] = {}
            for each in tree.getchildren():
                result = internal_iter(each, {})
                if each.tag in accum[tree.tag]:
                    if not isinstance(accum[tree.tag][each.tag], list):
                        accum[tree.tag][each.tag] = [
                            accum[tree.tag][each.tag]
                        ]
                    accum[tree.tag][each.tag].append(result[each.tag])
                else:
                    accum[tree.tag].update(result)
        else:
            accum[tree.tag] = tree.text

        return accum

    return internal_iter(element_tree, {})



def str_to_bool(s):
    if s == 'True' or s == 'true':
        return True
    elif s == 'False' or s == 'false':
        return False
    else:
        raise ValueError  # evil ValueError that doesn't tell you what the wrong value was


def run_cmd(work_dir, cmd, timeout_sec=0):
    df = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=work_dir)
    stdout_bytes, stderr_bytes = df.communicate()
    output = stdout_bytes.decode('utf-8')
    error = stderr_bytes.decode('utf-8')

    return df.returncode, output


def read_qor_json(test_path):
    print("INFO: Read golden json data")
    ret = 0

    qor_json = None
    qor_file = test_path + "/" + QOR_FILENAME

    if os.path.exists(qor_file) is False:
        # print("Not able to find the qor.json file: [" + test_path + "/" + QOR_FILENAME + "]")
        print("WARNING: The qor.json file does NOT exist. Please check the repo. Skip the QoR checking.")
        return 1, qor_json

    with open(qor_file) as qor:
        qor_string = qor.read()

        ret, error = is_json(qor_string)
        if ret is False:
            return 2, error

        qor_json = json.loads(qor_string)

    return 0, qor_json

###############################################
# re.search('<(.+?)>', a)
# tag = b.group(1)
# ss = "<" + tag + ">(.+?)</" + tag + ">"
# c = re.search(ss, a)
# c.group(1)
###############################################


def find_hls_xml_reports(test_path, xclbin=None, kernel=None):
    xml_report = None
    # print("Find HLS report files (xml)")
    # find ./ -name "*_csynth.xml"
    cmd = "find " + test_path + "/ -name \"*.xml\" | grep csynth"
    if kernel is None or kernel is False:
        cmd = ("find %s/ -maxdepth 8 -type f -name \"%s\"" % (test_path, HLS_XML_REPORT))
    else:
        cmd = ("find %s/ -maxdepth 8 -type f -name \"%s\" | grep %s" % (test_path, HLS_XML_REPORT, kernel))

    # f xclbin is not None and xclbin is not False:
    #    cmd = cmd + " | grep " + xclbin

    # print(cmd)
    ret, files = run_cmd(test_path, cmd)
    files = files.split('\n')
    files = [x for x in files if x]

    if len(files) == 0:
        print("WARNING: Cannot find xml report for kernel name: %s" % kernel)
        print("         Please make sure [qor.json] file has correct kernel name.")
        print("         Or check the run directory and make sure HLS csynth.xml report is generated.")
        return 1, xml_report

    if len(files) > 1:
        print("WARNING: Multiple xml reports found for kernel name: %s" % kernel)
        for xml_file in files:
            if xclbin in xml_file:
                xml_report = xml_file
                break

        if xml_report is None:
            xml_report = files[0]

        print("INFO: Selected xml report: %s" % (xml_report))
    else:
        xml_report = files[0]

    return ret, xml_report


def hls_xml2dic(xml_rpt):
    print("INFO: Parse HLS xml report and construct dictionary")

    xml_string = None
    with open(xml_rpt) as f:
        xml_string = f.read()

    # print("Read xml reports")
    dict_rpt = make_dict_from_tree(ET.fromstring(xml_string))

    return dict_rpt


def krnl_xml2dic(path, xclbin_name, kernel_name):
    print("INFO: Get xml report for kernel [" + kernel_name + "]")

    ret, krnl_xml_rpt = find_hls_xml_reports(path, xclbin_name, kernel_name)
    if ret != 0:
        return None

    # if (kernel_name):
    print("INFO: Kernel XML report: %s" % krnl_xml_rpt)

    # create element tree object
    krnl_xmldic = hls_xml2dic(krnl_xml_rpt)

    return krnl_xmldic


def get_dir_path():
    cwd = os.getcwd()
    if "/xbinst" in cwd:
        test_path = os.path.dirname(cwd)
        xbinst_path = cwd
    else:
        test_path = cwd
        xbinst_path = cwd + "/xbinst"

    return test_path, xbinst_path


def check_hls_warning(outputs):
    ret = 0
    for warning in outputs:
        print(warning)

    if len(outputs) > 0:
        ret = 1

    return ret


def check_hls_log(path, kname):
    cmd = ("find -maxdepth 5 -type f -name \"*%s\" | grep %s" % (HLS_LOG_FILE, kname))
    # print(cmd)
    ret, hls_logs = run_cmd(path, cmd)
    hls_logs = hls_logs.split('\n')
    hls_logs = [x for x in hls_logs if x]
    # print(hls_logs)

    is_fail = False

    for log in hls_logs:
        print("INFO:WARNING for [" + log + "]")
        cmd = "cat " + log + " | grep WARNING"
        ret, output = run_cmd(path, cmd)
        output = output.split('\n')
        output = [x for x in output if x]
        # print(output)

        ret = check_hls_warning(output)
        if is_fail is False:
            if ret > 0:
                is_fail = True
        print("")

    return is_fail, hls_logs


def check_sys_timing(path, cname=None):
    # Find xclbin file
    if cname is None or cname is False:
        cmd = "find " + path + " -name \"*.xclbin\""
    else:
        cmd = "find " + path + " -name \"" + cname + "*.xclbin\""
    ret, xclbins = run_cmd(path, cmd)

    xclbins = xclbins.split('\n')
    xclbins = [x for x in xclbins if x]
    # print(xclbins)

    if (len(xclbins) > 0):
        print("System timing met. xclbin files exist: (%d) file(s)." % len(xclbins))
        return False, len(xclbins), xclbins
    else:
        print("System timing does NOT met. (%d) file(s)." % len(xclbins))
        return True, len(xclbins), xclbins


def check_krnl_timing(krnl_dic):
    print("INFO: Check Kernel Timing")

    is_fail = False
    golden_perf_info = get_value("Kernel xml report", krnl_dic, USER_ASSIGNMENTS)
    golden_unit_info = get_value("Kernel xml report", golden_perf_info, UNIT)
    golden_clk_info  = get_value("Kernel xml report", golden_perf_info, TARGET_CLK_PERIOD, float)
    golden_clk_uncertain_info = get_value("Kernel xml report", golden_perf_info, CLK_UNCERTAINTY, float)

    est_perf_info = get_value("Kernel xml report", krnl_dic, PERF_EST)
    est_timing_info = get_value("Kernel xml report", est_perf_info, SUM_TIMING)
    est_unit_info = get_value("Kernel xml report", est_timing_info, UNIT)
    est_clk_info = get_value("Kernel xml report", est_timing_info, EST_CLK, float)

    if golden_unit_info != est_unit_info:
        print("WARNING: Unit is different. Need adjustment. (Target: %s, Estimated: %s)" % (golden_unit_info, est_unit_info))

    if golden_clk_info is False or est_clk_info is False:
        return False
    if (golden_clk_info < est_clk_info):
        print("WARNING: Does not meet target timing: (Target: %f, Estimated: %f)" % (golden_clk_info, est_clk_info))
        is_fail = True
    elif (est_clk_info == 0.0):
        print("WARNING: Estimated Clock period is %f: (Target: %f)" % (0.0, golden_clk_info))
    else:
        print("INFO: Timing: PASS (Target: %f(%3.2f), Estimated: %f)" % (golden_clk_info, golden_clk_uncertain_info, est_clk_info))

    return is_fail



def check_krnl_pipe_ii(golden_loop_ii, est_loop_ii, est_loop_ii_regex, loop_id, loop_name, use_regex_in):
    is_fail = False
    use_regex = use_regex_in

    # Get the golden information
    golden_ii = get_value("qor.json", golden_loop_ii, PIPE_II, int)
    est_ii = 900000000

    if use_regex is False:
        k, val = find_key(est_loop_ii, loop_name)
        if val is None:
            # print("Kernel xml report has no entry for [%s]. Try with regex" % loop_name)
            use_regex = True
        else:
            est_ii = get_value("Kernel xml report", val, PIPE_II, int)

    if use_regex is True:
        est_loop_regex_rule = ("{'Name': '%s'.+'%s':.+}" % (loop_name, PIPE_II))
        est_loop_regex = re.findall(re.compile(est_loop_regex_rule), est_loop_ii_regex)
        if len(est_loop_regex) > 0:
            est_loop_regex = est_loop_regex[0]
        else:
            est_loop_regex = None

        if est_loop_regex is None:
            return False

        est_ii_regex_rule = ("'(%s)': '([0-9]+)'" % PIPE_II)
        est_ii_regex = re.findall(re.compile(est_ii_regex_rule), est_loop_regex)
        if len(est_ii_regex) > 0:
            est_ii_regex = est_ii_regex[0]
        else:
            print("WARNING: Can't get the [%s:%s] info with regex" % (loop_id, PIPE_II))
            return False

        # Regex findall will give tuple like this: ('PipelineII', '1'). So, extract number and convert it int
        est_ii_regex = est_ii_regex[1]
        if est_ii_regex.isnumeric() is False:
            return False
        est_ii = int(est_ii_regex)

    if golden_ii is False or est_ii is False:
        return False

    ii_check_result = "PASS"
    if est_ii > golden_ii:
        print("WARNING: [%s] Estimated II can't reach the target II (Target: %d, Estimated: %d)" % (loop_name, golden_ii, est_ii))
        ii_check_result = "FAIL"
        is_fail = True

    if est_ii < golden_ii:
        print("WARNING: [%s] Estimated II is smaller than the target II (Target: %d, Estimated: %d)." % (loop_name, golden_ii, est_ii))
        print("WARNING: Please check the QoR setting for [%s]" % (loop_name))
        ii_check_result = "FAIL"
        is_fail = True

    print("INFO: Loop id: %d, %s II Check: %s (Golden II: %d, Estimated II: %d)" % (loop_id + 1, loop_name, ii_check_result, golden_ii, est_ii))

    return is_fail


def get_value(name, dic, key, dtype=None):

    if dic is False or dic is None:
        return False

    if key not in dic:
        if name is not None:
            print("WARNING: %s does not have information for [%s]" % (name, key))
        else:
            print("WARNING: Key [%s] doesn't exist." % (key))
        return False
    else:
        if dtype is bool:
            return str_to_bool(dic[key])
        elif dtype is int:
            return int(dic[key])
        elif dtype is float:
            return float(dic[key])
        else:
            return dic[key]


def check_qor(qor, path, test_mode):
    print("INFO: Start QoR Checking..")

    # qor_check = get_value("qor.json", qor, QOR_CHECK)
    # if qor_check is False:
    #    print("Do not check the QoR for this example. Exiting..")
    #    #return False

    containers = get_value("qor.json", qor, XCLBINS)
    if containers is False:
        print("Cannot find any xclbin container information")
        sys.exit(0)

    fail = False
    failure_msgs = []

    for idx, xclbin in enumerate(containers):
        container_name = get_value("qor.json", xclbin, NAME)
        if container_name is not False:
            print("INFO: Container name is: " + container_name)
        else:
            print("No xclbin container name in qor.json")

        sys_timing = get_value("qor.json", xclbin, CHK_SYS_TIMING, bool)
        if '_emu_' in path:
            print("Skip System Timing Check for Emulation")
            sys_timing = False

        # Check sys timing
        if sys_timing is True:
            print("INFO: Check System Timing")
            is_fail, _, _ = check_sys_timing(path, container_name)

            if is_fail is True:
                fail = fail or is_fail
                failure_msgs.append("sys_timing")

        accelerators = get_value("qor.json", xclbin, KERNELS)
        if accelerators is False:
            continue

        # Per kernel checking
        print("INFO: Check Each Kernel")
        for kid, kernel in enumerate(accelerators):
            kname = get_value("qor.json", kernel, NAME)
            if kname is not False:
                print("INFO: Kernel name: " + kname)

                # Get kernel xml report and create dictionary
                krnl_dic = krnl_xml2dic(path, container_name, kname)
                if krnl_dic is None:
                    print("WARNING: Can't get the xml report for %s" % kname)
                    fail = True
                    msg = ("Can't get the xml report for [%s]" % kname)
                    failure_msgs.append(msg)
                    continue

                krnl_dic = get_value("Kernel xml report", krnl_dic, PROFILE)
                if krnl_dic is False:
                    print("WARNING: Kernel XML report does not have [%s] information" % PROFILE)
                    continue

                timing = get_value("qor.json", kernel, CHK_TIMING, bool)
                if timing is True:
                    is_fail = check_krnl_timing(krnl_dic)

                    if is_fail is True:
                        fail = fail or is_fail
                        msg = kname + "_timing"
                        failure_msgs.append(msg)

                est_perf = get_value("Kernel xml report", krnl_dic, PERF_EST)
                if est_perf is False:
                    print("WARNING: Kernel XML report does not have [%s] information" % PERF_EST)
                    continue

                est_perf_regex_rule = ("('%s': {'SummaryOfTimingAnalysis.+\}\}\}), '" % PERF_EST)
                est_perf_regex = re.findall(re.compile(est_perf_regex_rule), str(krnl_dic))
                if len(est_perf_regex) > 0:
                    est_perf_regex = est_perf_regex[0]
                else:
                    est_perf_regex = None

                do_ii_check = True
                use_regex = False

                golden_loop_ii = get_value("qor.json", kernel, LOOPS)
                if golden_loop_ii is False:
                    print("WARNING: Can't extract [%s] information from kernel XML report" % LOOPS)

                est_loop_ii = get_value("Kernel xml report", est_perf, SUM_LOOP_LAT)
                if est_loop_ii is False:
                    print("WARNING: Can't extract [%s] information from kernel XML report" % SUM_LOOP_LAT)

                est_loop_ii_regex_rule = ("'%s': {.+}" % SUM_LOOP_LAT)
                est_loop_ii_regex = re.findall(re.compile(est_loop_ii_regex_rule), est_perf_regex)
                if len(est_loop_ii_regex) > 0:
                    est_loop_ii_regex = est_loop_ii_regex[0]
                    print("INFO: Extracted [%s] information from kernel XML report w/ regex" % SUM_LOOP_LAT)
                else:
                    est_loop_ii_regex = None
                    print("WARNING: Can't extract [%s] information from kernel XML report w/ regex" % SUM_LOOP_LAT)

                if golden_loop_ii is False or (est_loop_ii is False and est_loop_ii_regex is None):
                    print("WARNING: Can't perform PipelineII Check due to missing information")
                    do_ii_check = False

                # import pdb; pdb.set_trace()
                if do_ii_check is not False:
                    print("INFO: Check Loop PipelineII")

                    if est_loop_ii is False:
                        print("INFO: Use regex to extract PipelineII info")
                        use_regex = True

                    for lid, loop in enumerate(golden_loop_ii):
                        lname = get_value("qor.json:loops", loop, NAME)
                        # print("INFO: Loop #%d: %s" % (lid+1, lname))
                        if lname is False:
                            continue
                        is_fail = check_krnl_pipe_ii(loop, est_loop_ii, est_loop_ii_regex, lid, lname, use_regex)
                        if is_fail is True:
                            fail = fail or is_fail
                            msg = kname + "_" + lname + "_ii"
                            failure_msgs.append(msg)

                latency = get_value("qor.json", kernel, CHK_LATENCY, bool)
                # if latency is True:
                #    print(" Check latency")

                pipetype = get_value("qor.json", kernel, CHK_PIPETYPE)
                # if pipetype is not False:
                #    print(" Check PipelineType")

                warning = get_value("qor.json", kernel, CHK_WARNING, bool)
                if warning:
                    print("INFO: Check HLS warning")
                    is_fail, _ = check_hls_log(path, kname)
                    # print(is_fail)
                    if is_fail is True:
                        if failure_msgs_ON_HLS_WARNING is True and test_mode == 'hw_emu':
                            fail = fail or is_fail
                        else:
                            print("INFO: QoR failure_msgs with HLS warning is disabled for now.")
                        msg = ("WARNING: HLS warning for %s" % kname)
                        failure_msgs.append(msg)
            print("")

    if fail is True:
        result_msg = "WARNING"
        if test_mode == 'hw_emu':
            result_msg = "ERROR"
        if len(failure_msgs) > 0:
            print("%s: QoR FAILED for:" % result_msg)
            for msg in failure_msgs:
                print("- %s" % msg)
        else:
            print("%s: QoR Check FAILED\n" % result_msg)

    else:
        print("INFO: QoR Check PASS\n")

    return fail


if __name__ == '__main__':
    print("\nChecking ONBOARD/GITHUB examples QoR")

    parser = ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument("-m", "--mode", dest="test_mode", default='hw', required=False, choices=['sw_emu', 'hw_emu', 'hw'], help="Flow for the run")
    args = parser.parse_args()


    is_fail = False

    test_path, xbinst_path = get_dir_path()
    # print(test_path)
    # print(xbinst_path)
    # ret, files = find_hls_xml_reports(test_path)
    exit_code = 0
    ret, qor = read_qor_json(test_path)
    if ret != 0:
        message_head = "WARNING"
        if args.test_mode == 'hw_emu' and ret == 2:
            message_head = "ERROR"
            exit_code = ret

        if ret == 2:
            print("%s: Can't parse QoR JSON file: %s\n" % (message_head, str(qor)))
        sys.exit(exit_code)

    is_fail = check_qor(qor, test_path, args.test_mode)

    exit_code = 0
    if is_fail is True:
        if args.test_mode == 'hw_emu':
            print("ERROR: HW emulation. HLS QoR check failed.")
            exit_code = 1
        else:
            print("WARNING: HLS QoR check failed. Please check the log file.")

    sys.exit(exit_code)

