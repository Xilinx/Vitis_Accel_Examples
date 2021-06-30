#!/usr/bin/env python

import os, re
import fnmatch
import json


def get_testcases(dir):
  testcase_list = []
  for root, dirnames, filenames in os.walk(dir):
    for filename in fnmatch.filter(filenames, 'description.json'):
      testcase_list.append(os.path.join(root, filename))
  return testcase_list

def get_drives(dir):
  folders = []
  while 1:
    dir, folder = os.path.split(dir)
    if folder != "" and folder != ".":
        folders.append(folder)
    else:
        break
  folders.reverse()
  return folders

def get_immediate_subdirectories(dir):
    return [name for name in os.listdir(dir)
            if os.path.isdir(os.path.join(dir, name))]

def gen_category(dir ,outfile, subdircount, ref_data):

  links = "[" + dir +"]:"+ dir + "\n"
  testcaselist = get_testcases(dir);
  testcaselist.sort();
  for testcase in testcaselist:
    drives = get_drives(testcase)
    link = ""
    if len(drives) <= subdircount : 
      continue
    for drive in drives:
      if drive == "description.json":
        continue
      link += drive +"/"
    links += "[" + link + "]:" + link + "\n"
    
    outfile.write("["+link+"][]")
    outfile.write("|")
    desc = open(testcase,'r')
    data = json.load(desc)
    outfile.write(('\n').join(data["description"]))
    outfile.write("|")
    if 'key_concepts' in data:
        outfile.write("__Key__ __Concepts__")
        key_concepts = data["key_concepts"]
        for i, kc in enumerate(key_concepts):
            outfile.write("<br>")
            outfile.write(" - ")
            if "key_concepts" in ref_data:
                if kc in ref_data["key_concepts"]:
                    outfile.write("[")
                    outfile.write(kc)
                    outfile.write("](")
                    outfile.write(ref_data["key_concepts"][kc])
                    outfile.write(")")
                else:
                    outfile.write(kc)
        outfile.write("<br>")
    if 'keywords' in data:    
        outfile.write("__Keywords__")
        keywords = data["keywords"]
        for  i, kw in enumerate(keywords):
            outfile.write("<br>")
            outfile.write(" - ")
            if "keywords" in ref_data:
                if kw in ref_data["keywords"]:
                    outfile.write("[")
                    outfile.write(kw)
                    outfile.write("](")
                    outfile.write(ref_data["keywords"][kw])
                    outfile.write(")")
                else:
                    outfile.write(kw)
    outfile.write("\n")
    desc.close()
  return links

def genReadMe(dir):
  desc = open(os.path.join(dir,"summary.json"),'r')
  data = json.load(desc)
  outfile = open(os.path.join(dir, "README.md"), "w")
  outfile.write(('\n').join((data["description"])))
  outfile.write("\n")
  outfile.write("==================================\n")
  outfile.write(('\n').join((data["description"])))
  outfile.write("\n")
  if 'subdirs' in data:
    subDirs = data['subdirs'];
  else:
    subDirs = get_immediate_subdirectories(dir);
    subDirs.sort();
  outfile.write("\nS.No.   | Category  | Description \n")
  outfile.write("--------|-----------|-----------------------------------------\n")
  counter = 1;
  links = ""
  
  for subDir in subDirs:
    desc_file = os.path.join(subDir,"summary.json")
    if os.path.exists(desc_file):
        subDirDesc = open(os.path.join(subDir,"summary.json"),'r')
        subDirData = json.load(subDirDesc)
        outfile.write(str(counter));
        outfile.write(" | [" +subDir +"][]      |")
        outfile.write(('\n').join(subDirData["description"]))
        outfile.write("\n")
        counter = counter + 1;
      
  outfile.write("\n __Examples Table__ \n")
  table_header = """
Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
"""
  outfile.write(table_header)
  
  file_name = "LICENSE.txt" # file to be searched
  cur_dir = os.getcwd()      # Dir from where search starts can be replaced with any path
  init_cur_dir = cur_dir
  ref_data = ""
  
  while True:
      file_list = os.listdir(cur_dir)
      parent_dir = os.path.dirname(cur_dir)
      if file_name in file_list:
          path = cur_dir + '/common/utility/reference.json'
          ref_file = open(path,'r')
          ref_data = json.load(ref_file)
          ref_file.close()
          break
      else:
          if cur_dir == parent_dir:         # if dir is root dir
              print ("LICENSE.txt file not found")
              break
          else:
            cur_dir = parent_dir
  
  for subDir in subDirs:
    links = links + gen_category(subDir,outfile,2,ref_data);

  outfile.write("\n")
  outfile.write(links)
  outfile.close();

def genReadMe2(dir):
  desc = open(os.path.join(dir,"summary.json"),'r')
  data = json.load(desc)
  outfile = open(os.path.join(dir, "README.md"), "w")
  outfile.write(('\n').join((data["overview"])))
  outfile.write("\n")
  outfile.write("==================================\n")
  outfile.write(('\n').join((data["description"])))
  outfile.write("\n")
  outfile.write("\n __Examples Table__ \n")
  table_header = """
Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
"""
  outfile.write(table_header)
  
  file_name = "LICENSE.txt" # file to be searched
  cur_dir = os.getcwd()      # Dir from where search starts can be replaced with any path
  init_cur_dir = cur_dir
  ref_data = ""
  
  while True:
      file_list = os.listdir(cur_dir)
      parent_dir = os.path.dirname(cur_dir)
      if file_name in file_list:
          path = cur_dir + '/common/utility/reference.json'
          ref_file = open(path,'r')
          ref_data = json.load(ref_file)
          ref_file.close()
          break
      else:
          if cur_dir == parent_dir:         # if dir is root dir
              print ("LICENSE.txt file not found")
              break
          else:
              cur_dir = parent_dir

  links = gen_category(dir,outfile,1,ref_data)
  outfile.write("\n")
  outfile.write(links)
  outfile.close();

