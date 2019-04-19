//
// v++(TM)
// rundef.js: a v++-generated Runs Script for WSH 5.1/5.6
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//

echo "This script was generated under a different operating system."
echo "Please update the PATH variable below, before executing this script"
exit

var WshShell = new ActiveXObject( "WScript.Shell" );
var ProcEnv = WshShell.Environment( "Process" );
var PathVal = ProcEnv("PATH");
if ( PathVal.length == 0 ) {
  PathVal = "/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/SDK/2019.1/bin;/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Scout/2019.1/bin;/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/bin;";
} else {
  PathVal = "/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/SDK/2019.1/bin;/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Scout/2019.1/bin;/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/bin;" + PathVal;
}

ProcEnv("PATH") = PathVal;

var RDScrFP = WScript.ScriptFullName;
var RDScrN = WScript.ScriptName;
var RDScrDir = RDScrFP.substr( 0, RDScrFP.length - RDScrN.length - 1 );
var ISEJScriptLib = RDScrDir + "/ISEWrap.js";
eval( EAInclude(ISEJScriptLib) );


ISEStep( "vivado_hls",
         "-f vadd.tcl -messageDb vivado_hls.pb" );



function EAInclude( EAInclFilename ) {
  var EAFso = new ActiveXObject( "Scripting.FileSystemObject" );
  var EAInclFile = EAFso.OpenTextFile( EAInclFilename );
  var EAIFContents = EAInclFile.ReadAll();
  EAInclFile.Close();
  return EAIFContents;
}
