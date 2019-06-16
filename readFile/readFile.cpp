#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility> // pair
#include <regex>
#include <algorithm>
#include <cmath>

//d::string g_sFilename = "../data/wanhao-i3-plus-air-duct-v6_0.15mm_PLA_WANA.gcode";
std::string g_sFilename = "../data/v7.gcode";
//d::string g_sFilename = "../data/v6.gcode";
//d::string g_sFilename = "../data/GT2_Belt_Tensioner_-_Belt_Side_0.15mm_PLA_PRIMA.gcode";

using CTokens  = std::vector<std::string>;
using CCodes   = std::vector<std::pair<char, double>>;

using APosition = std::array<double, 3>;

APosition g_previousPosition{0,0,0};
double xi{0}, xa{0}, yi{0}, ya{0}, zi{0}, za{0}, e{0}, me{0}, mn{0};

struct SCommand
    {
    using CFilterMap = std::map<std::string, std::function<void(SCommand const &)>>;

    std::string sCmd;
    CCodes vc{};
    double X{0};
    double Y{0};
    double Z{0};
    double E{0};

    ~SCommand()
        {
        g_previousPosition = {X,Y,Z};
        };

    void append(std::string const & crs)
        {
        if (sCmd == "") {sCmd = crs; return; }

        auto c = crs[0];
        auto d = std::stod(crs.substr(1));
/*!*/   if ( c == 'F' ) 
            if ( d > 450 )
                return; // ignoreIt;
        if ( c == 'X' ) X = d;
        if ( c == 'Y' ) Y = d;
        if ( c == 'Z' ) Z = d;
        if ( c == 'E' ) E = d;

        vc.emplace_back(std::make_pair(c, d));
        }

    void send()
        {
        auto found = filters.find(sCmd);
        if (found != filters.end())
            {
            found->second(*this);
            }
        else
            {
            NullFilter(*this);
            }                                                                                                             
        }

    CFilterMap filters = {
//      { "G0"  , G0  }, // Linear Move w/o extraction     check for F-eedrate against Firmware
        { "G1"  , G1  }, // Linear Move with extraction    check for F-eedrate against Firmware
//      { "G21" , G21 }, // Millimeter Units
//      { "G90" , G90 }, // Absolute Positioning
//      { "G91" , G91 }, // Relative Positioning
//      { "G92" , G92 }, // Set Position
 
//      { "M82" , M82 }, // E Absolute
//      { "M83" , M83 }, // E Relative
//      { "M104", M104}, // Set Hotend Temperature
//      { "M106", M106}, // Set Fan Speed
//      { "M109", M109}, // Wait for Hotend Temperature
//      { "M140", M140}, // Set Bed Temperature
//      { "M190", M190}, // Wait for Bed Temperature
//      { "M201", M201}, // Set Print Max Acceleration
//      { "M203", M203}, // Set Max Feedrate
//      { "M204", M204}, // Set Starting Acceleration
//      { "M205", M205}, // Set Advanced Settings
//      { "M900", M900}  // Linear Advance Factor
        };

    static void NullFilter(SCommand const & cro) {};

    static void G0  (SCommand const & cro) {}; // Linear Move w/o extraction     check for F-eedrate against Firmware

    static void G1  (SCommand const & cro)     // Linear Move with extraction    check for F-eedrate against Firmware
        {
        for ( auto const & a:cro.vc )
            {
            auto x = cro.X-g_previousPosition[0];
            auto y = cro.Y-g_previousPosition[1];
            auto z = cro.Z-g_previousPosition[2];
            if ( cro.E != 0 ) // potential move with extrusion
                {
                me += std::sqrt( x*x + y*y + z*z )/2;
                }
            else
                {
                mn += std::sqrt( x*x + y*y + z*z )/2;
                }
            switch (a.first)
                {
                case 'X': xi=std::min(xi, a.second/2+100); xa=std::max(xa, a.second/2+100); break;
                case 'Y': yi=std::min(yi, a.second/2+300); ya=std::max(ya, a.second/2+300); break;
                case 'Z': zi=std::min(zi, a.second/2+000); za=std::max(za, a.second/2+000); break;
                case 'E': e += a.second/2; break;
                }
            }
        }
    
    static void G21 (SCommand const & cro) {}; // Millimeter Units
    static void G90 (SCommand const & cro) {}; // Absolute Positioning
    static void G91 (SCommand const & cro) {}; // Relative Positioning
    static void G92 (SCommand const & cro) {}; // Set Position
 
    static void M82 (SCommand const & cro) {}; // E Absolute
    static void M83 (SCommand const & cro) {}; // E Relative
    static void M104(SCommand const & cro) {}; // Set Hotend Temperature
    static void M106(SCommand const & cro) {}; // Set Fan Speed
    static void M109(SCommand const & cro) {}; // Wait for Hotend Temperature
    static void M140(SCommand const & cro) {}; // Set Bed Temperature
    static void M190(SCommand const & cro) {}; // Wait for Bed Temperature
    static void M201(SCommand const & cro) {}; // Set Print Max Acceleration
    static void M203(SCommand const & cro) {}; // Set Max Feedrate
    static void M204(SCommand const & cro) {}; // Set Starting Acceleration
    static void M205(SCommand const & cro) {}; // Set Advanced Settings
    static void M900(SCommand const & cro) {}; // Linear Advance Factor

    }; // struct SCommand


CTokens Tokenize(std::string const & sLine)
    {
    std::regex re("\\s+");
    CTokens oTokens{std::sregex_token_iterator(sLine.begin(), sLine.end(), re, -1), {}};
    SCommand oCommand{};

    for (auto const & a:oTokens)
        {
        oCommand.append(a);
  //    std::cout << c << "=" << d << ";";
        }
    oCommand.send();
 // std::cout << "\n";
    return oTokens;
    }

// G1 X74.603 Y78.250 E0.03570

// auto oM203 = Tokenize("M203 X450.00 Y450.00 Z5.00 E25.00");

int main(int argc, char* argv[])
    {
    std::ifstream fGCode(g_sFilename.c_str()/*, std::ios::in*/);
    std::string   sLine;

    while( std::getline(fGCode, sLine))
        {
        size_t n=sLine.find_first_of(';');
        if ( n!=std::string::npos ) sLine.erase(n);
        if (sLine.length()>0)
            {
            auto ot{Tokenize(sLine)};
            }
        }

    std::cout << "x[" << xi << ";" << xa << "]; "
              << "y[" << yi << ";" << ya << "]; "
              << "z[" << zi << ";" << za << "]; "
              << "e=" << e  << "; "
              << "E=" << me << "; "
              << "N=" << mn << "\n";

    return 0;
    }

/*

m92 e140
g1 f50 e5

==============================================================================

# cat Z3-RaederKoppelGetriebe-5+2_0.15mm_PLA_KOSSEL.gcode|sed -rn '/^G[0-9]+ /p'|sed -r 's/^(G[0-9]+).*$/\1/'|sort -u
G0
G1
G21
G90
G92

# cat Z3-RaederKoppelGetriebe-5+2_0.15mm_PLA_KOSSEL.gcode|sed -rn '/^M[0-9]+ /p'|sed -r 's/^(M[0-9]+).*$/\1/'|sort -u
M104
M106
M109
M140
M190
M201
M203
M204
M205
M82
M900

==============================================================================

WANHAO i3 plus - PID calibration 230°C

M117 Changing EEPROM
M206 T0 P216 S1
M206 T0 P217 S255
G4 P1000
M117 Running PID Tune
G4 P2000
M303 S230 X1
M400
M117 PID Tune Saved
G4 P5000

==============================================================================

WANHAO i3 plus - PID calibration 220°C

M117 Changing EEPROM
M206 T0 P216 S1
M206 T0 P217 S255
G4 P1000
M117 Running PID Tune
G4 P2000
M303 S220 X1
M400
M117 PID Tune Saved
G4 P5000

==============================================================================

PRIMA CREATOR

SENDING:M503
Steps per unit:
M92 X93.00 Y93.00 Z1097.50 E97.00
Maximum feedrates (mm/s):
M203 X150.00 Y150.00 Z1.50 E50.00
Maximum Acceleration (mm/s2):
M201 X800 Y800 Z20 E10000
Accelerations: P=printing, R=retract and T=travel
M204 P3000.00 R3000.00 T3000.00
Advanced variables:
S=Min feedrate (mm/s),
T=Min travel feedrate (mm/s),
B=minimum segment time (ms),
X=maximum XY jerk (mm/s),
Z=maximum Z jerk (mm/s),
E=maximum E jerk (mm/s)
M205 S0.00 T0.00 B20000 X20.00 Z0.40 E5.00
Home offset (mm):
M206 X0.00 Y0.00 Z0.00
Invert axis: M562 XYZE
XYZABCD-+-+-+- 
PID settings:  
M301 P20.00 I0.02 D250.00 C100.00 L20
M304 P10.00 I0.02 D305.40
Filament settings: Disabled
M200 D1.75     
M200 D0        
ok N0 P15 B15  

==============================================================================

WANHAO I3 PLUS

No port specified - connecting to /dev/ttyUSB1 at 115200bps
start          
Printer is now online
Marlin 1.0.0   0°> 
Last Updated: Jun  7 2017 12:59:05 | Author: (none, default config)
Compiled: Jun  7 2017
Free Memory: 3679  PlannerBufferBytes: 1232
Stored settings retrieved
Steps per unit:0°> 
M92 X81.00 Y81.00 Z400.00 E140.00
Maximum feedrates (mm/s):
M203 X450.00 Y450.00 Z5.00 E25.00
Maximum Acceleration (mm/s2):
M201 X3000 Y3000 Z100 E3000
Acceleration: S=acceleration, T=retract acceleration
M204 S1000.00 T800.00
Advanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)
M205 S0.00 T0.00 B20000 X10.00 Z0.40 E1.00
Home offset (mm):> 
M206 X0.00 Y0.00 Z0.00
PID settings:  0°> 
M301 P33.00 I1.00 D-59.00
SD card ok     0°> 
enqueing "M84" 0°> 

==============================================================================

; generated by Slic3r Prusa Edition 1.41.0 on 2019-06-07 at 00:53:38

; 

; external perimeters extrusion width = 0.45mm
; perimeters extrusion width = 0.45mm
; infill extrusion width = 0.45mm
; solid infill extrusion width = 0.45mm
; top infill extrusion width = 0.45mm
; first layer extrusion width = 0.42mm

M107
M190 S55 ; set bed temperature and wait for it to be reached
M104 S245 ; set temperature
G28 ; home all axes\n
G1 Z5 F5000 ; lift nozzle\n
M900 K200; Filament gcode
M109 S245 ; set temperature and wait for it to be reached
G21 ; set units to millimeters
G90 ; use absolute coordinates
M83 ; use relative distances for extrusion

G1 E-4.00000 F1200.00000
G1 Z0.600 F12000.000
G1 X74.096 Y79.038
G1 Z0.250
G1 E4.00000 F1200.00000
M204 S1000
G1 X74.603 Y78.250 E0.03570
G1 X74.984 Y77.919 E0.01920
G1 X75.426 Y77.668 E0.01934
G1 X75.905 Y77.511 E0.01920
G1 X76.409 Y77.451 E0.01934
G1 X116.584 Y77.450 E1.52978
G1 X117.531 Y77.634 E0.03673
G1 X118.345 Y78.160 E0.03688
G1 X118.904 Y78.950 E0.03688


!   +   G0-G1: Linear Move
     +  G2-G3: Controlled Arc Move
        G4: Dwell
     +  G5: Bézier cubic spline
    +   G10: Retract
    +   G11: Recover/Unretract
    +   G12: Clean the Nozzle
    +   G20: Inch Units
    +   G21: Millimeter Units
        G26: Mesh Validation Pattern
    +   G27: Park toolhead
!   +   G28: Auto Home
    +   G29: Bed Leveling (Manual)
    +   G29: Bed Leveling (Automatic)
    +   G29: Bed Leveling (Unified)
        G30: Single Z-Probe
        G31: Dock Sled
        G32: Undock Sled
    +   G33: Delta Auto Calibration
        G38.2-G38.3: Probe target
        G42: Move to mesh coordinate
        G53: Move in Machine Coordinates
    ?   G54-G55: Workspace Coordinate System
!   +   G90: Absolute Positioning
!   +   G91: Relative Positioning
!   +   G92: Set Position
        G425: Perform auto-calibration
    +   M0-M1: Unconditional stop
        M3: Spindle CW / Laser On
        M4: Spindle CCW / Laser On
        M5: Spindle / Laser Off
    +   M17: Enable Steppers
    +   M18-M84: Disable steppers
    +   M20: List SD Card
        M21: Init SD card
        M22: Release SD card
    +   M23: Select SD file
    +   M24: Start or Resume SD print
    +   M25: Pause SD print
    +   M26: Set SD position
    +   M27: Report SD print status
    +   M28: Start SD write
    +   M29: Stop SD write
    +   M30: Delete SD file
    +   M31: Print time
    +   M32: Select and Start
        M33: Get Long Path
    +   M34: SDCard Sorting
    +   M42: Set Pin State
        M43: Debug Pins
        M43 T: Toggle Pins
        M48: Probe Accuracy Test
        M73: Set Print Progress
    +   M75: Start Print Job Timer
    +   M76: Pause Print Job
    +   M77: Stop Print Job Timer
    +   M78: Print Job Stats
    +   M80: Power On
    +   M81: Power Off
!   +   M82: E Absolute
!   +   M83: E Relative
    +   M85: Inactivity Shutdown
    +   M92: Set Axis Steps-per-unit
    +   M100: Free Memory
!   +   M104: Set Hotend Temperature
!   +   M105: Report Temperatures
    +   M106: Set Fan Speed
!   +   M107: Fan Off
!   +   M108: Break and Continue
!   +   M109: Wait for Hotend Temperature
        M110: Set Line Number
     +  M111: Debug Level
    +   M112: Emergency Stop
    +   M113: Host Keepalive
    +   M114: Get Current Position
    +   M115: Firmware Info
    +   M117: Set LCD Message
    +   M118: Serial print
    +   M119: Endstop States
    +   M120: Enable Endstops
    +   M121: Disable Endstops
     +  M122: TMC Debugging
    +   M125: Park Head
     +  M126: Baricuda 1 Open
     +  M127: Baricuda 1 Close
     +  M128: Baricuda 2 Open
     +  M129: Baricuda 2 Close
!   +   M140: Set Bed Temperature
    +   M145: Set Material Preset
!   +   M149: Set Temperature Units
    +   M150: Set RGB(W) Color
    +   M155: Temperature Auto-Report
      + M163: Set Mix Factor
      + M164: Save Mix
      + M165: Set Mix
      + M166: Gradient Mix
!     + M190: Wait for Bed Temperature
    +   M200: Set Filament Diameter
    +   M201: Set Print Max Acceleration
    +   M203: Set Max Feedrate
    +   M204: Set Starting Acceleration
    +   M205: Set Advanced Settings
    +   M206: Set Home Offsets
    +   M207: Set Firmware Retraction
    +   M208: Firmware Recover
    +   M209: Set Auto Retract
    +   M211: Software Endstops
    +   M217: Filament swap parameters
    +   M218: Set Hotend Offset
    +   M220: Set Feedrate Percentage
    +   M221: Set Flow Percentage
    +   M226: Wait for Pin State
    +   M240: Trigger Camera
    +   M250: LCD Contrast
     +  M260: I2C Send
     +  M261: I2C Request
     +  M280: Servo Position
     +  M290: Babystep
    +   M300: Play Tone
      + M301: Set Hotend PID
    +   M302: Cold Extrude
    +   M303: PID autotune
    +   M304: Set Bed PID
      + M350: Set micro-stepping
      + M351: Set Microstep Pins
    +   M355: Case Light Control
        M360: SCARA Theta A
        M361: SCARA Theta-B
        M362: SCARA Psi-A
        M363: SCARA Psi-B
        M364: SCARA Psi-C
      + M380: Activate Solenoid
      + M381: Deactivate Solenoids
    +   M400: Finish Moves
     +  M401: Deploy Probe
     +  M402: Stow Probe
     +  M403: MMU2 Filament Type
    +   M404: Set Filament Diameter
    +   M405: Filament Width Sensor On
    +   M406: Filament Width Sensor Off
    +   M407: Filament Width
    +   M410: Quickstop
    +   M412: Filament Runout
    +   M413: Power-loss Recovery
     +  M420: Bed Leveling State
     +  M421: Set Mesh Value
     +  M425: Backlash compensation
    +   M428: Home Offsets Here
    +   M500: Save Settings
    +   M501: Restore Settings
    +   M502: Factory Reset
    +   M503: Report Settings
     +  M504: Validate EEPROM contents
    +   M524: Abort SD print
    +   M540: Endstops Abort SD
     +  M569: Set TMC stepping mode
    +   M600: Filament Change
    +   M603: Configure Filament Change
     +  M605: Dual Nozzle Mode
      + M665: Delta Configuration
        M665: SCARA Configuration
      + M666: Set Delta endstop adjustments
      + M666: Set dual endstop offsets
    +   M701: Load filament
    +   M702: Unload filament
    +   M851: Z Probe Offset
      + M852: Bed Skew Compensation
    +   M900: Linear Advance Factor
     +  M906: TMC Motor Current
    +   M907: Set Motor Current
      + M908: Set Trimpot Pins
      + M909: DAC Print Values
      + M910: Commit DAC to EEPROM
      + M911: TMC OT Pre-Warn Condition
      + M912: Clear TMC OT Pre-Warn
      + M913: Set Hybrid Threshold Speed
      + M914: TMC Bump Sensitivity
      + M915: TMC Z axis calibration
     +  M928: Start SD Logging
     +  M999: STOP Restart
        M7219: MAX7219 Control
    

*/
