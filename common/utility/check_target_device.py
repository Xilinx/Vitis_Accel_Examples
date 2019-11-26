import json, sys

descriptionfile = sys.argv[1]
target = sys.argv[2]
device = sys.argv[3]

with open(descriptionfile) as json_file:
    data = json.load(json_file)

targetNotSupported = 'targets' in data and target not in data['targets']
if targetNotSupported:
    print("%s target not supported for this example" % target)

deviceNotSupported = 'nboard' in data and any(nboard in device for nboard in data['nboard'])
if deviceNotSupported:
    print("%s device not supported for this example" % device)

sys.exit(not(targetNotSupported or deviceNotSupported))
