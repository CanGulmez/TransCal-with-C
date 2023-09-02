/* The DC and AC Analysis of JFET Transistor Configurations

JFET transistor have a large usage area in electronics and related
discipline. So, I've written this source file which contain mostly
used JFET transistor calculations. 

Before using this source file, please, read these important notes. 
I hope, it will be usefull...

IMPORTANT NOTES:
----------------

1. When you run any method, results will display automatically,
apart from that, you can reach each result in related struct.
2. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
3. All transistor configuratiions are set as 'npn' type. 
4. In ac analysis, algorithms use 'JFET small signal' model.

EXISTING CONFIGURATIONS:
------------------------

+ dc_fixed_bias()
+ ac_fixed_bias()
+ dc_self_bias()
+ ac_self_bias()
+ dc_voltage_divider()
+ ac_voltage_divider()
+ dc_common_gate()
+ ac_common_gate()
+ ac_source_follower()
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Results of DC analysis
struct DCResults {
   float Id; // drain current
   float Vgs; // gate-source voltage
   float Vds; // drain-source voltage
   float Vs; // source voltage
   float Vd; // drain voltage
   float Vg; // gate voltage
} DCAnalysis;

// Results of AC analysis
struct ACResults {
   float gm; // transconductance factor
   float Zi; // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   char *phase; // phase relationship
} ACAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

void _save_dc_results_(float Id, float Vds, float Vgs, float Vs, 
                       float Vd, float Vg) {
   // Save the DC results into 'DCAnalysis' struct.
   DCAnalysis.Id = Id; // drain current
   DCAnalysis.Vds = Vds; // drain-source voltage
   DCAnalysis.Vgs = Vgs; // gate-source voltage
   DCAnalysis.Vs = Vs; // source voltage
   DCAnalysis.Vd = Vd; // drain voltage
   DCAnalysis.Vg = Vg; // gate voltage
}

void _save_ac_results_(float gm, float Zi, float Zo, float Av, 
                       char *phase) {
   // Save the AC results into 'ACAnalysis' struct.
   ACAnalysis.gm = gm; // transconductance factor
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationship
}

void _display_dc_results_(void) {
   // Display the DC results.
   printf("Id: %e A\n", DCAnalysis.Id);
   printf("Vgs: %f V\n", DCAnalysis.Vgs);
   printf("Vds: %f V\n", DCAnalysis.Vds);
   printf("Vg: %f V\n", DCAnalysis.Vg);
   printf("Vd: %f V\n", DCAnalysis.Vd);
   printf("Vs: %f V\n", DCAnalysis.Vs);
}

void _display_ac_results_(void) {
   // Display the AC results.
   printf("gm: %e S\n", ACAnalysis.gm);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("Phase: %s\n", ACAnalysis.phase);
}

float _parallel_(float R1, float R2) {
   // Find the resultant resistance for parallel resistors.
   return 1.0 / (1.0 / R1 + 1.0 / R2);
}

float _find_gm_factor_(float Idss, float Vp, float Vgs) {
   // Find the transconductance factor (gm).
   return (2.0 * Idss / abs(Vp)) * (1.0 - Vgs / Vp);
}

float _select_right_Id_(float a, float b, float c) {
   // Find the discriminant and calculate two different roots.
   float dicriminant = (b * b) - (4 * a * c);
   float root1 = (-1.0 * b + sqrt(dicriminant)) / (2 * a);
   float root2 = (-1.0 * b - sqrt(dicriminant)) / (2 * a);
   // Specially, in some configurations, can be found two 
   // roots and requries to select one of them.
   if (root1 >= 0 && root2 < 0) return root1;
   if (root2 >= 0 && root1 < 0) return root2;
   if (root1 >= 0 && root2 >= 0) {
      if (root1 >= root2) return root2;
      else return root1;} 
   if (root1 < 0 && root2 < 0) {
      if (abs(root1) >= abs(root2)) return abs(root2);
      else return abs(root1);}
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

'Vdd', 'Vgg', 'Rg' and 'Rd' parameters stand for drain voltage,
gate voltage, gate resistance and drain resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. All current and resistance values must be in 
form of 'A' and 'ohm'.
*/
void dc_fixed_bias(float Vdd, float Vgg, float Rg, float Rd, 
                   float Idss, float Vp) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of fixed-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Vgs = -1 * Vgg; // gate-source voltage
   float Id = Idss * (1.0-Vgs/Vp) * (1.0-Vgs/Vp); // drain current
   float Vds = Vdd - Id * Rd; // drain-source voltage
   float Vd = Vds; // drain voltage
   float Vg = Vgs; // gate voltage
   float Vs = 0; // source voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   _display_dc_results_(); 
}

/* AC analysis of fixed-bias transistor configuration. 

'Vdd', 'Vgg', 'Rg' and 'Rd' parameters stand for drain voltage,
gate voltage, gate resistance and drain resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All 
current and resistance values must be in form of 'A' and 'ohm'.
*/
void ac_fixed_bias(float Vdd, float Vgg, float Rg, float Rd, 
                   float Idss, float Vp, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of fixed-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Vgs = -1 * Vgg; // gate-source voltage
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm
   float Zi = Rg; // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av = -1.0 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
   _display_ac_results_(); 
}

/* DC analysis of self-bias transistor configuration. 

'Vdd', 'Rg', 'Rd', 'Rs' parameters stand for drain voltage, gate
resistance, drain resistance and source resistance in order. 'Idss'
parameter is the maximum drain current. 'Vp' parameter is the
pinch-off voltage. All current and resistance values must be in 
form of 'A' and 'ohm'.
*/
void dc_self_bias(float Vdd, float Rg, float Rd, float Rs, 
                  float Idss, float Vp) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0 || Rs <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of self-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using dicriminant.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b = Idss * 2.0 * Rs / Vp - 1;
   float c = Idss;
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = -1 * Id * Rs; // gate-source voltage
   float Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   float Vs = Id * Rs; // source voltage
   float Vg = 0; // gate voltage
   float Vd = Vds + Vs; // drain voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   _display_dc_results_(); 
}

/* AC analysis of self-bias transistor configuration. 

'Vdd', 'Rg', 'Rd', 'Rs' parameters stand for drain voltage, gate
resistance, drain resistance and source resistance in order. 'Idss'
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All
current and resistance values must be in form of 'A' and 'ohm'.
*/
void ac_self_bias(float Vdd, float Rg, float Rd, float Rs, 
                  float Idss, float Vp, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0 || Rs <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of self-bias config. are inconsistent.");

   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using discriminant.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b = Idss * 2.0 * Rs / Vp - 1;
   float c = Idss;
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = -1 * Id * Rs; // gate-source voltage
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi = Rg; // input impedance
   float Zo1 = 1 + gm * Rs + Rs / rd;
   float Zo2 = 1 + gm * Rs + Rs / rd + Rd / rd;
   float Zo = Zo1 * Rd / Zo2; // output impedance
   float Av1 = gm * Rd;
   float Av2 = 1.0 + gm * Rs + (Rd + Rs) / rd;
   float Av = -1.0 * Av1 / Av2; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
   _display_ac_results_(); 
}

/* DC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd', 'Rs' parameters stand for drain voltage,
upper gate resistance, lower gate resistance, drain resistance and
source resistance. 'Idss' parameter is the maximum drain current. 
'Vp' parameter is the pinch-off voltage. All current and resistance 
values must be in form of 'A' and 'ohm'.
*/
void dc_voltage_divider(float Vdd, float Rg1, float Rg2, float Rd,
                        float Rs, float Idss, float Vp) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg1 <= 0 || Rs <= 0 || Rg2 <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of voltage-divider config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b1 = (2.0 * Rs * Idss / Vp);
   float b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   float b = b1 - b2 - 1;
   float c = Idss * (1.0-(2.0 * Vg/Vp) + (Vg * Vg/Vp/Vp));
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs;  // gate-source voltage
   float Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   float Vs = Id * Rs; // source voltage
   float Vd = Vdd - Id * Rd; // drain voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   _display_dc_results_(); 
}

/* AC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd', 'Rs' parameters stand for drain voltage,
upper gate resistance, lower gate resistance, drain resistance and
source resistance. 'Idss' parameter is the maximum drain current. 
'Vp' parameter is the pinch-off voltage. 'rd' parameter is the drain 
resistance. All current and resistance values must be in form 
of 'A' and 'ohm'.
*/
void ac_voltage_divider(float Vdd, float Rg1, float Rg2, float Rd,
                        float Rs, float Idss, float Vp, float rd){
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg1 <= 0 || Rg2 <= 0 || Rs <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of voltage-divider config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b1 = (2.0 * Rs * Idss / Vp);
   float b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   float b = b1 - b2 - 1;
   float c = Idss * (1.0-(2.0 * Vg/Vp) + (Vg * Vg/Vp/Vp));
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs;  // gate-source voltage
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi = _parallel_(Rg1, Rg2); // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
   _display_ac_results_(); 

}

/* DC analysis of common-gate transistor configuration. 

'Vdd', 'Vss', 'Rd', 'Rs' parameters stand for drain voltage, 
source voltage, drain resistance and source resistance. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. All current and resistance must be in form 
of 'A' and 'ohm'. 
*/
void dc_common_gate(float Vdd, float Vss, float Rd, float Rs, 
                    float Idss, float Vp) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rs <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of common-gate config. are inconsistent.");

   // For quadritic equations, find discriminant.
   float a = (Rs * Rs) * Idss / (Vp * Vp);
   float b1 = 2.0 * Rs * Idss / Vp;
   float b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   float b = b1 + b2 - 1.0;
   float c1 = 2.0 * Vss / Vp;
   float c2 = Vss * Vss / Vp / Vp;
   float c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vss - Id * Rs; // gate-source voltage
   float Vds = Vdd + Vss - Id * (Rs + Rd); // drain-source voltage
   float Vs = -Vss + Id * Rs; // source voltage
   float Vd = Vdd - Id * Rd; // drain voltage
   float Vg = 0; // gate voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   _display_dc_results_(); 
}

/* AC analysis of common-gate transistor configuration. 

'Vdd', 'Vss', 'Rd', 'Rs' parameters stand for drain voltage, 
source voltage, drain resistance and source resistance. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All 
current and resistance must be in form of 'A' and 'ohm'. 
*/
void ac_common_gate(float Vdd, float Vss, float Rd, float Rs, 
                    float Idss, float Vp, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rs <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of common-gate config. are inconsistent.");

   // For quadritic equations, find discriminant.
   float a = (Rs * Rs) * Idss / (Vp * Vp);
   float b1 = 2.0 * Rs * Idss / Vp;
   float b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   float b = b1 + b2 - 1.0;
   float c1 = 2.0 * Vss / Vp;
   float c2 = Vss * Vss / Vp / Vp;
   float c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vss - Id * Rs; // gate-source voltage
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi1 = (rd + Rd) / (1 + gm * rd);
   float Zi = _parallel_(Rs, Zi1); // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av1 = gm * Rd + Rd / rd;
   float Av2 = 1 + Rd / rd;
   float Av = Av1 / Av2; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "In phase");
   _display_ac_results_(); 
}

/* AC analysis of source-follower transistor configuration. 

'Vdd', 'Vgs', 'Rg', 'Rs' parameters stand for drain voltage, gate-
source voltage, gate resistance, source resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' paramter is the drain resistance.
*/
void ac_source_follower(float Vdd, float Vgs, float Rg, float Rs, 
                        float Idss, float Vp, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rg <= 0 || Rs <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "JFETError", __LINE__, 
      "Parameters of source-follower config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float gm = _find_gm_factor_(Idss, Vp, Vgs);
   float Zi = Rg;
   float Zo = _parallel_(rd, _parallel_(Rs, 1/gm));
   float Av1 = gm * _parallel_(rd, Rs);
   float Av2 = 1.0 + Av1;
   float Av = Av1 / Av2;

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "In phase");
   _display_ac_results_(); 
}
