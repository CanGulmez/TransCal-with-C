/* The DC and AC Analysis of BJT Transistor Configurations

BJT transistor have a large usage area in electronics and related
discipline. So, I've written this source file which contain mostly
used BJT transistor calculations. 

Before using this source file, please, read these important notes. 
I hope, it will be usefull...

IMPORTANT NOTES:
----------------

1. If you see any result as -1.0, probably, the algorithms 
cannot calculate that result.
2. When you run any method, results will display automatically,
apart from that, you can reach each result in related struct.
3. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
4. All transistor configuratiions are set as 'npn' type. 
5. In ac analysis, algorithms use 're transistor' model.

EXISTING CONFIGURATIONS:
------------------------

+ dc_fixed_bias()
+ ac_fixed_bias()
+ dc_emitter_bias()
+ ac_emitter_bias()
+ dc_voltage_divider()
+ ac_voltage_divider()
+ dc_collector_feedback()
+ ac_collector_feedback()
+ ac_collector_dc_feedback()
+ dc_emitter_follower()
+ ac_emitter_follower()
+ dc_common_base()
+ ac_common_base()
+ dc_miscellaneous_bias()
+ two_port_system()
+ cascaded_system()
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// General constants
#define Vbe 0.7
#define MAX_TRANSISTOR 64

// Result of DC analysis
struct DCResults {
   float Ib; // base current
   float Ic; // collector current
   float Ie; // emitter current
   float Icsat; // collector saturation (max) current
   float Vce; // collector-emitter voltage
   float Vc; // collector voltage
   float Ve; // emitter voltage
   float Vb; // base current
   float Vbc; // base-collector voltage
} DCAnalysis;

// Results of AC analysis
struct ACResults {
   float re; // re factor
   float Zi;  // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   char* phase; // phase relationship
} ACAnalysis;

// Results of Two Port System
struct TwoPortResults {
   float Avl; // load-voltage gain
   float Avs; // source-voltage gain
   float Ail; // load-current gain
} TwoPortAnalysis;

// Result of Cascaded System
struct CascadedResults {
   char stage_names[MAX_TRANSISTOR][10]; // stage names
   float stage_volts[MAX_TRANSISTOR]; // stage voltages
   float Avt; // total voltage gain
   float Avs; // source voltage gain
   float Ait; // total current gain
} CascadedAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

float _Rth_(float R1, float R2) {
   // Rth is necesarry for voltage divider config.
   return 1 / (1/R1 + 1/R2); 
}

float _Eth_(float Vcc, float R1, float R2) {
   // Eth is necesarry for voltage divider config.
   return Vcc * (R2 / (R1 + R2));
}

void _save_dc_results_(float Ib, float Ic, float Ie, float Icsat,
                       float Vce, float Vc, float Ve, float Vb, 
                       float Vbc) {
   // Save the DC results into the 'DCAnalysis' struct.
   DCAnalysis.Ib = Ib; // base current
   DCAnalysis.Ic = Ic; // collector current
   DCAnalysis.Ie = Ie; // emitter current
   DCAnalysis.Icsat = Icsat; // collector saturation (max) current
   DCAnalysis.Vce = Vce; // collector-emitter voltage
   DCAnalysis.Vc = Vc; // collector voltage
   DCAnalysis.Ve = Ve; // emitter voltage
   DCAnalysis.Vb = Vb; // base voltage
   DCAnalysis.Vbc = Vbc; // base-collector voltage
}

void _save_ac_results_(float re, float Zi, float Zo, float Av,
                       char* phase) {
   // Save the AC results into the 'ACAnalysis' struct.
   ACAnalysis.re = re; // re factor
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationships
}

void _save_two_port_results_(float Avl, float Avs, float Ail) {
   // Save the results of the two port systems.
   TwoPortAnalysis.Avl = Avl; // load-voltage gain
   TwoPortAnalysis.Avs = Avs; // source-voltage gain
   TwoPortAnalysis.Ail = Ail; // load-current gain
}

void _display_dc_results_(void) {
   // Display the DC analysis results. An important point is 
   // that if and result seems as -1 and then that result is
   // not calculated. So, result will be wrong value.
   printf("Ib: %e A\n", DCAnalysis.Ib);
   printf("Ic: %e A\n", DCAnalysis.Ic);
   printf("Ie: %e A\n", DCAnalysis.Ie);
   printf("Ic(sat): %f A\n", DCAnalysis.Icsat);
   printf("Vce: %f V\n", DCAnalysis.Vce);
   printf("Vc: %f V\n", DCAnalysis.Vc);
   printf("Ve: %f V\n", DCAnalysis.Ve);
   printf("Vb: %f V\n", DCAnalysis.Vb);
   printf("Vbc: %f V\n", DCAnalysis.Vbc);
   printf("Vbe: %f V\n", Vbe);
}

void _display_ac_results_(void) {
   // Display the DC analysis results. An important point is 
   // that if and result seems as -1 and then that result is
   // not calculated. So, result will be wrong value.
   printf("re: %f ohm\n", ACAnalysis.re);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("phase: %s\n", ACAnalysis.phase);
}

void _display_two_port_results_(void) {
   // Display the results of two port systems.
   printf("Avl: %f\n", TwoPortAnalysis.Avl);
   printf("Avs: %f\n", TwoPortAnalysis.Avs);
   printf("Ail: %f\n", TwoPortAnalysis.Ail);
}

void _raise_error_(char *file, char *error, int line, char *msg) {
   // Display the error message if there is a any error existance.
   printf("In %s::%d, found an error.\n", file, line);
   printf("%s: %s\n", error, msg);
   exit(EXIT_FAILURE);
}

/* --------------------------------------------------------------- */
/* ------------------------ Main Definations --------------------- */
/* --------------------------------------------------------------- */

/* DC analysis of fixed-bias transistor configuration. 

'Vcc', 'Rb', 'Rc' parameters stand for collector voltage, base
resistance and collector resistance. 'beta' patameter is the ratio 
of transistor's collector current to its base current. All resistor
values must be in form of 'ohm'.
*/
void dc_fixed_bias(float Vcc, float Rb, float Rc, float beta) {

   // Check if parameters of transistor are consistent.
   if (Rb <= 0 || Rc <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of fixed-bias config. are inconsistent.");
   
   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / Rb; // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / Rc; // collector saturation current
   float Vce = Vcc - (Ic * Rc); // collector-emitter voltage
   float Vc = Vce; // collector voltage
   float Ve = 0; // emitter voltage
   float Vb = Vbe; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* AC analysis of fixed-bias transistor configuration. 

'Vcc', 'Rb', 'Rc' parameters stand for collector voltage, base
resistance and collector resistance. 'beta' patameter is the ratio 
of transistor's collector current to its base current. 'ro' is the 
output impedance of transistor. All resistor values must be in 
form of 'ohm'.
*/
void ac_fixed_bias(float Vcc, float Rb, float Rc, float beta, 
                   float ro) {
   // Check if parameters of transistor are consistent.
   if (Rb <= 0 || Rc <= 0 || beta <= 0 || ro <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of fixed-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / Rb; // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Rb + 1/(beta * re)); // input impedance
   float Zo = 1 / (1/Rc + 1/ro); // output impedance
   float Av = -1 * (1 / (1/Rc + 1/ro)) / re; // voltage gain

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
   _display_ac_results_();
}

/* DC analysis of emitter-bias transistor configuration. 

'Vcc', 'Rb', 'Rc', 'Re' parameters stand for collector voltage, 
base resistance, collector resistance and emitter resistance. 
'beta' parameter is the ratio of the transistor's collector 
current to its base current. All resistor values must be in form 
of 'ohm'.
*/
void dc_emitter_bias(float Vcc, float Rb, float Rc, float Re, 
                     float beta) {
   // Check if parameters of transistor are consistent.
   if (Rb <= 0 || Rc <= 0 || beta <= 0 || Re <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of emitter-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.  
   float Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* AC analysis of emitter-bias transistor configuration. 

'Vcc', 'Rb', 'Rc', 'Re' parameters stand for collector voltage, 
base resistance, collector resistance and emitter resistance. 'beta'
parameter is the ratio of the transistor's collector current to its 
base current. 'ro' parameter is the output impedance of the 
transistor. All resistor values must be in form of 'ohm'.
*/
void ac_emitter_bias(float Vcc, float Rb, float Rc, float Re, 
                     float beta, float ro) {
   // Check if parameters of transistor are consistent.
   if (Rb <= 0 || Rc <= 0 || beta <= 0 || Re <= 0 || ro <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of emitter-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zb1 = (beta + 1) + (Rc/ro);
   float Zb2 = 1 + (Rc + Re) / ro;
   float Zb = beta * re + (Zb1 / Zb2) * Re;
   float Zi = 1 / (1/Rb + 1/Zb); // input impedance
   float Zo1 = beta * (ro + re);
   float Zo2 = 1 + (beta * re) / Re;
   float Zo3 = ro + Zo1 / Zo2;
   float Zo = 1 / (1/Rc + 1/Zo3); // output impedance
   float Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
   float Av2 = 1 + (Rc / ro);
   float Av = Av1 / Av2; // voltage gain

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
   _display_ac_results_();
}

/* DC analysis of voltage-divider transisfor configuration. 

'Vcc', 'Rb1', 'Rb2', 'Rc', 'Re' parameters stand for collector
voltage, upper base resistance, lower base resistance, collector
resistance and emitter resistance. 'beta' parameter is the ratio of
transistor's collector current to its base current. All resistor
values must be in form of 'ohm'.
*/
void dc_voltage_divider(float Vcc, float Rb1, float Rb2, float Rc, 
                        float Re, float beta) {
   // Check if parameters of transistor are consistent.
   if (Rb1 <= 0 || Rb2 <= 0 || Rc <= 0 || Re <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of voltage-divider config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float rth = _Rth_(Rb1, Rb2); // thevenin resistor
   float eth = _Eth_(Vcc, Rb1, Rb2); // thevenin voltage
   float Ib = (eth - Vbe) / (rth +(beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter current
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* AC analysis of voltage-divider transistor configuration. 

'Vcc', 'Rb1', 'Rb2', 'Rc', 'Re' parameters stand for collector
voltage, upper base resistance, lower base resistance, collector
resistance and emitter resistance. 'beta' parameter is the ratio 
of transistor's collector current to its base current. 'ro' 
parameter is the output impedance of the transistor. 'bypass' 
parameter must be  'bypassed' or 'unbypassed'. That define the 
bypass status of the emitter terminal. All resistor values must 
be in form of 'ohm'.
*/
void ac_voltage_divider(float Vcc, float Rb1, float Rb2, float Rc, 
                        float Re,float beta,float ro,char* bypass) {
   // Check if parameters of transistor are consistent.
   if (Rb1 <= 0 || Rb2 <= 0 || Rc <= 0 || Re <= 0 || beta <= 0 ||
      ro <= 0) _raise_error_(__FILE__, "BJTError", __LINE__,
      "Parameters of voltage-divider config. are inconsistent.");

   // Indicate the all variables.
   float Zb1, Zb2, Zb, Zi, Zo1, Zo2, Zo3, Zo, Av1, Av2, Av;
   // Calculate the all analyzes of transistor.   
   float rth = _Rth_(Rb1, Rb2); // thevenin resistor 
   float eth = _Eth_(Vcc, Rb1, Rb2); // thevenin voltage
   float Ib = (eth - Vbe) / (rth +(beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor

   // According to 'bypass' parameter, there are two options.
   if (strcmp(bypass, "bypassed") == 0) {
      Zi = 1 / (1 / rth + 1 / (beta * re)); // input impedance
      Zo = 1 / (1 / Rc + 1 / ro); // output impedance
      Av = -1 * _Rth_(Rc, ro) / re; // voltage gain
   } 
   else if (strcmp(bypass, "unbypassed") == 0) {
      Zb1 = (beta + 1) + (Rc/ro);
      Zb2 = 1 + (Rc + Re) / ro;
      Zb = beta * re + (Zb1 / Zb2) * Re;
      Zi = 1 / (1/rth + 1/Zb); // input impedance
      Zo1 = beta * (ro + re);
      Zo2 = 1 + (beta * re) / Re;
      Zo3 = ro + Zo1 / Zo2;
      Zo = 1 / (1/Rc + 1/Zo3); // output impedance
      Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
      Av2 = 1 + (Rc / ro);
      Av = Av1 / Av2; // voltage gain
   }
   else _raise_error_(__FILE__, "BJTError", __LINE__, 
      "'bypass' parameter must be 'bypassed' or 'unbypassed'.");

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
   _display_ac_results_();
}

/* DC analysis of collector-feedback transistor configuration. 

'Vcc', 'Rf', 'Rc', 'Re' parameters stand for collector voltage, 
base  resistance, collector resistance and emitter resistance. 
'beta' parameter is the ratio of transistor's collector current 
to its base current. All resistor values must be in form of 'ohm'.
*/
void dc_collector_feedback(float Vcc, float Rf, float Rc, float Re, 
                           float beta) {  
   // Check if parameters of transistor are consistent.
   if (Rf <= 0 || Rc <= 0 || Re <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of collector-feedback config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rf + beta * (Rc+Re)); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* AC analysis of collector-feedback transistor configuration. 

'Vcc', 'Rf', 'Rc' parameters stand for collector voltage, base 
resistance and collector resistance. 'beta' parameter is the ratio 
of the transistor's collector current to its base current. 'ro' 
parameter is the output impedance of transistor. All resistor values
must be in form of 'ohm'.
*/
void ac_collector_feedback(float Vcc, float Rf, float Rc, float beta,
                           float ro) {
   // Check if parameters of transistor are consistent.
   if (Rf <= 0 || Rc <= 0 || beta <= 0 || ro <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of collector-feedback config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rf + beta * Rc); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zi1 = 1 + (_Rth_(Rc, ro) / Rf);
   float Zi2 = (1 / (beta * re)) + (1 / Rf);
   float Zi3 = _Rth_(Rc, ro) / (beta * re * Rf);
   float Zi4 = _Rth_(Rc, ro) / (Rf * re);
   float Zi = Zi1 / (Zi2 + Zi3 + Zi4); // input impedance
   float Zo = 1 / (1/ro + 1/Rc + 1/Rf); // output impedance
   float Av1 = Rf / (_Rth_(Rc, ro) + Rf);
   float Av2 = _Rth_(Rc, ro) / re;
   float Av = -1 * Av1 * Av2; // voltage gain

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
   _display_ac_results_();
}

/* AC analysis of collector-dc-feedback transistor configuration. 

'Vcc', 'Rf1', 'Rf2', 'Rc' parameters stand for collector voltage, 
first base resistance, second base resistance and collecor 
resistance. 'beta' parameter is the ratio of the transistor's 
collector current to its base current. 'ro' parameter is the output
impedance of the transistor. All resistors must be in form of 'ohm'.
*/
void ac_collector_dc_feedback(float Vcc, float Rf1, float Rf2, 
                              float Rc, float beta, float ro) {
   // Check if the parameters of transistor are consistent.
   if (Rf1 <= 0 || Rf2 <= 0 || Rc <= 0 || beta <= 0 || ro <= 0) 
   _raise_error_(__FILE__, "BJTError", __LINE__, 
   "Parameters of collector-dc-feedback config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rf1+Rf2 + (beta * Rc)); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Rf1 + 1/(beta * re)); // input impedance
   float Zo = 1 / (1/Rc + 1/Rf2 + 1/ro); // output impedance
   float Av = -1 * Zo / re; // voltage gain

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
   _display_ac_results_();
}

/* DC analysis of emitter-follower transisfor configuration. 

'Vee', 'Rb, 'Re' parameters stand for emitter voltage, base 
resistance and emitter resistance. 'beta' parameter is the ratio 
of the transistor's collector current to its base current. All 
resistor values must be in form of 'ohm'.
*/
void dc_emitter_follower(float Vee, float Rb, float Re, float beta) {
   // Check if the parameters of transistor are consistent.
   if (Rb <= 0 || Re <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of emitter follower config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vee - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float Ic = beta * Ib; // collector current
   float Vce = Vee - (Ie * Re); // collector-enitter voltage
   float Ve = (Ie * Re) + Vee; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* AC analysis of emitter-follower transistor configuration. 

'Vcc', 'Rb', 'Re' parameters stand for collector voltage, base
resistance and emitter resistance in order. 'beta' parameter is 
the ratio of the transistor's current to its base current. 'ro' 
parameter is the output resistor of the transistor. All resistor 
values must  be in form of 'ohm'.
*/
void ac_emitter_follower(float Vcc, float Rb, float Re, float beta,
                         float ro) {
   // Check if the parameters of transistor are consistent.
   if (Rb <= 0 || Re <= 0 || beta <= 0 || ro <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of emitter-follower config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zb1 = (beta + 1) * Re;
   float Zb2 = 1 + (Re / ro);
   float Zb = (beta * re) + (Zb1 / Zb2);
   float Zi = 1 / (1 /Rb + 1 /Zb); // input impedance
   float Zo1 = (beta * re) / (beta + 1);
   float Zo = 1 / (1 /ro + 1 /Re + 1 /Zo1); // output impedance
   float Av1 = (beta + 1) * Re / Zb;
   float Av = Av1 / (1 + (Re/ro)); // voltage gain

   // Save and display the ac results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "In phase");
   _display_ac_results_();
}

/* DC analysis of common-base transisfor configuration. 

''Vcc', 'Vee', 'Rc' and 'Re' parameters stand for collector voltage, 
emitter voltage, collector resistance and emitter resistance. All 
resistor values must be in form of 'ohm'. 'beta' parameter  is the 
ratio of the transistor's collector current to its base current. 
*/
void dc_common_base(float Vcc, float Vee, float Rc, float Re,
                    float beta) {
   // Check if the parameters of transistor are consistent.
   if (Rc <= 0 || Re <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of common-base config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ie = (Vee - Vbe) / Re; // emitter current
   float Ib = Ie / (beta + 1); // base current
   float Ic = Ib * beta; // collector current
   float Vce = Vee + Vcc - Ie * (Rc + Re); // collector-emitter v.
   float Vcb = Vcc - Ic * Rc; // collector-base voltage
   float Vbc = -1 * Vcb; // base-collector voltage

   // Save and display the results of transistor.
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, -1.0, -1.0, -1.0, Vbc);
   _display_dc_results_();
}

/* AC analysis of common-base transistor configuration. 

'Vcc', 'Vee', 'Rc' and 'Re' parameters stand for collector voltage, 
emitter voltage, collector resistance and emitter resistance. All 
resistor values must be in form of 'ohm'. 'alpha' parameter is the 
current gain of common-base configuration.
*/
void ac_common_base(float Vcc, float Vee, float Rc, float Re, 
                    float alpha) {
   // Check if the parameters of transistor are consistent.
   if (Rc <= 0 || Re <= 0 || alpha <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of common-base config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ie = (Vee - Vbe) / Re; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Re + 1/re); // input impedance
   float Zo = Rc; // output impedance
   float Av = alpha * Rc / re; // voltage gain

   // Save and display the results of transistor.
   _save_ac_results_(re, Zi, Zo, Av, "In phase");
   _display_ac_results_();
}

/* DC analysis of miscellaneous-bias transisfor configuration. 

'Vcc', 'Rb', 'Rc' parameters stnd for collector voltage, base
resistance and collector resistance in order. All resistors must 
be in  form of 'ohm'. 'beta' parameter is the ratio of collector 
current to its base current.
*/
void dc_miscellaneous_bias(float Vcc, float Rb, float Rc, 
                           float beta) {
   // Check if the parameters of transistor are consistent.
   if (Rb <= 0 || Rc <= 0 || beta <= 0) 
      _raise_error_(__FILE__, "BJTError", __LINE__, 
      "Parameters of miscellaneous-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Ib = (Vcc - Vbe) / (Rb + beta * Rc); // base current
   float Ic = beta * Ib; // collector current
   float Ie = (beta + 1) * Ib; // emitter current
   float Vce = Vcc - (Ie * Rc); // collector-emitter voltage
   float Ve = 0; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage

   // Save and display the results of transistor.
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
   _display_dc_results_();
}

/* Two Port System Analysis 

'Avnl', 'Zi' and 'Zo' parameters stand for non-load voltage gain, 
the input impedance and output impedance of your two port system 
in order. 'Rs' and 'Rl' parameters stand for source and load 
resistances. All resistors must be in form of 'ohm'. 
*/
void two_port_system(float Avnl, float Zi, float Zo, float Rs, 
                     float Rl) {
   // Check if the parameters of two port system are consistent.
   if (Zi <= 0 || Zo <= 0 || Rs <= 0 || Rl <= 0) 
      _raise_error_(__FILE__, "TwoPortSystemError", __LINE__, 
      "Parameters of two port system are inconsistent.");

   // Calculate the all analyzes of two port system.
   float Avl = Rl / (Rl + Zo) * Avnl; // load-voltage gain
   float Avs = Zi / (Zi + Rs) * Avl; // source-voltage gain
   float Ail = -1 * Avl * Zi / Rl; // load-current gain

   // Save and display the results of two port system.
   _save_two_port_results_(Avl, Avs, Ail);
   _display_two_port_results_();
}

/* Cascaded System Analysis 

'num' stand for number of transistor in your cascaded system. 
'Avnls', 'Zis', 'Zos' stand for non-load voltage gains, input 
impedances and output impedendeces in order. These parameters 
must be given as list to algorithm. 'Rs' and 'Rl' mean source 
and load resistors. All resistors must be in form of 'ohm'. 
*/
void cascaded_system(int num, float Avnls[num], float Zis[num], 
                     float Zos[num], float Rs, float Rl) {
   // Check if the parameters of cascaded system are consistent.
   for (int i=0; i<num; i++) 
      if (Zis[i] <= 0 || Zos[i] <= 0 || Rs <= 0 || Rl <= 0) 
         _raise_error_(__FILE__, "CascadedSystemError", __LINE__, 
         "Parameters of cascaded system are inconsistent.");

   float tvolt = 1; // total voltage gain
   // Calculate the all analyzes of cascaded system.
   for (int index=0; index < num-1; index++) {

      // Calculate the voltage gain in each stage.
      float Av = Zis[index + 1] / (Zis[index + 1] + Zos[index]);
      Av = Av * Avnls[index];
      // Append the voltage gain onto total voltage gain.
      tvolt = tvolt * Av;

      // Right know, create the stage voltage gain name.
      char iname[MAX_TRANSISTOR]; char vname[] = "Av";
      // To do that, previously, convert index to string, 
      // and than concatenate the strings.
      sprintf(iname, "%d", (index + 1)); strcat(vname, iname);
      // Append the stage names and voltages names into struct.
      strcpy(CascadedAnalysis.stage_names[index], vname);
      CascadedAnalysis.stage_volts[index] = Av;
      // Display the stage voltage gains.
      printf("%s: %f\n", vname, Av);
   }
   // Calculate the last stage voltage gain.
   float last_stage = Rl / (Rl + Zos[num-1]) * Avnls[num-1];
   // Append the last stage voltage gain onto total voltage gain.
   tvolt *= last_stage;
   // Create the last stage voltage name.
   char iname[MAX_TRANSISTOR]; char vname[] = "Av";
   sprintf(iname, "%d", num); strcat(vname, iname);

   // Append the last stage voltage gain into struct.
   strcpy(CascadedAnalysis.stage_names[num-1], vname);
   CascadedAnalysis.stage_volts[num-1] = last_stage;
   // Append the total voltage gain into struct.
   CascadedAnalysis.Avt = tvolt;
   // Append the source voltage gain into struct.
   CascadedAnalysis.Avs = Zis[0] / (Zis[0] + Rs) * tvolt;
   // Append the total current gain into struct.
   CascadedAnalysis.Ait = -1 * tvolt * Zis[0] / Rl;

   // Display the another voltage and current gains.
   printf("%s: %f\n", vname, last_stage);
   printf("Avt: %f\n", CascadedAnalysis.Avt);
   printf("Avs: %f\n", CascadedAnalysis.Avs);
   printf("Ait: %f\n", CascadedAnalysis.Ait);
}
