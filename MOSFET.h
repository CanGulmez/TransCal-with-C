/* The DC and AC Analysis of E-MOSFET Transistor Configurations

MOSFET transistor have a large usage area in electronics and related
discipline. So, I've written this source file which contain mostly
used E-MOSFET transistor calculations. I've not defined the D-MOSFET
calculations. Because its analyzes are the same with JFET. So, in 
this source code, there are two main different E-MOSFET analzes.

Before using this source file, please, read these important notes. 
I hope, it will be usefull...

IMPORTANT NOTES:
----------------

1. When you run any method, results will display automatically,
apart from that, you can reach each result in related struct.
2. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
3. All transistor configuratiions are set as 'npn' type. 

EXISTING CONFIGURATIONS:
------------------------

+ dc_drain_feedback()
+ ac_drain_feedback()
+ dc_voltage_divider()
+ ac_voltage_divider()
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Results of DC analysis
struct DCResults {
   float k; // k constant
   float Id; // drain current
   float Vgs; // gate-source voltage
   float Vds; // drain-gate voltage
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

void _save_dc_results_(float k, float Id, float Vds, float Vgs) {
   // Save the DC results into 'DCAnalysis' struct.
   DCAnalysis.k = k; // k constant
   DCAnalysis.Id = Id; // drain current
   DCAnalysis.Vds = Vds; // drain-source voltage
   DCAnalysis.Vgs = Vgs; // gate-source voltage
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
   printf("k: %e A/V^2\n", DCAnalysis.k);
   printf("Id: %e A\n", DCAnalysis.Id);
   printf("Vgs: %f V\n", DCAnalysis.Vgs);
   printf("Vds: %f V\n", DCAnalysis.Vds);
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

/* DC analysis of drain-feedback transistor configuration. 

'Vdd', 'Rg' and 'Rd' parameters stand for drain voltage, gate and
drain resistances. 'Idon' and 'Vgson' parameters are the threshold 
voltage and current level. 'Vgsth' parameter is the minimum value
to run the transistor. All resistances and currents must be in 
form of 'A' and 'ohm'.
*/
void dc_drain_feedback(float Vdd, float Rg, float Rd, float Idon, 
                       float Vgson, float Vgsth) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0) 
      _raise_error_(__FILE__, "MOSFETError", __LINE__, 
      "Parameters of drain-feedback config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   float a = Rd * Rd * k;
   float b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   float c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vdd - Id * Rd; // gate-source voltage
   float Vds = Vgs; // drain-source voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(k, Id, Vds, Vgs);
   _display_dc_results_(); 
}

/* AC analysis of drain-feedback transistor configuration. 

'Vdd', 'Rg' and 'Rd' parameters stand for drain voltage, gate and
drain resistances. 'Idon' and 'Vgson' parameters are the threshold 
voltage and current level. 'Vgsth' parameter is the minimum value
to run the transistor. 'rd' parameter is the drain resistance. All 
resistances and currents must be in form of 'A' and 'ohm'.
*/
void ac_drain_feedback(float Vdd, float Rg, float Rd, float Idon, 
                       float Vgson, float Vgsth, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "MOSFETError", __LINE__, 
      "Parameters of drain-feedback config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   float a = Rd * Rd * k;
   float b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   float c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vdd - Id * Rd; // gate-source voltage
   float gm = 2 * k * (Vgs - Vgsth); // gm factor
   float Zi1 = Rg + _parallel_(rd, Rd);
   float Zi2 = 1 + gm * _parallel_(rd, Rd); 
   float Zi = Zi1 / Zi2; // input impedance
   float Zo1 = _parallel_(rd, Rd);
   float Zo = _parallel_(Rg, Zo1); // output impedance
   float Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
   _display_ac_results_(); 
}

/* DC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd' and 'Rs' parameters stand for drain 
voltage, upper gate resistance, lower gate resistance, drain
resistance, source resistance in order. 'Idon' and 'Vgson' 
parameters are the threshold voltage and current level. 'Vgsth' 
parameter is the minimum value to run the transistor. All 
resistances and currents must be in form of 'A' and 'ohm'.
*/
void dc_voltage_divider(float Vdd, float Rg1, float Rg2, float Rd, 
                  float Rs, float Idon, float Vgson, float Vgsth) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg1 <= 0 || Rg2 <= 0 || Rs <= 0) 
      _raise_error_(__FILE__, "MOSFETError", __LINE__, 
      "Parameters of voltage-divider config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   float Vg = Rg2 * Vdd / (Rg1 + Rg2);
   float a = Rs * Rs * k;
   float b = -2 * k * Rs * (Vg - Vgsth) - 1;
   float c = k * (Vgsth - Vg) * (Vgsth - Vg);
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs; // gate-source voltage
   float Vds = Vdd - Id * (Rs + Rd); // drain-source voltage

   // Save and display the DC results of transistor.
   _save_dc_results_(k, Id, Vds, Vgs);
   _display_dc_results_(); 
}

/* AC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd' and 'Rs' parameters stand for drain 
voltage, upper gate resistance, lower gate resistance, drain
resistance, source resistance in order. 'Idon' and 'Vgson' 
parameters are the threshold voltage and current level. 'Vgsth' 
parameter is the minimum value to run the transistor. 'rd' 
parameter is the drain resistance. All resistances and currents 
must be in form of 'A' and 'ohm'.
*/
void ac_voltage_divider(float Vdd, float Rg1, float Rg2, float Rd, 
         float Rs, float Idon, float Vgson, float Vgsth, float rd) {
   // Check if the parameters of transistor are consistent.
   if (Rd <= 0 || Rg1 <= 0 || Rg2 <= 0 || Rs <= 0 || rd <= 0) 
      _raise_error_(__FILE__, "MOSFETError", __LINE__, 
      "Parameters of voltage-divider config. are inconsistent.");

   // Calculate the all analyzes of transistor.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   float Vg = Rg2 * Vdd / (Rg1 + Rg2);
   float a = Rs * Rs * k;
   float b = -2 * k * Rs * (Vg - Vgsth) - 1;
   float c = k * (Vgsth - Vg) * (Vgsth - Vg);
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs; // gate-source voltage
   float gm = 2 * k * (Vgs - Vgsth); // gm factor
   float Zi = _parallel_(Rg1, Rg2); // input impedance
   float Zo = _parallel_(rd, Rd); // output impedance
   float Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
   _display_ac_results_(); 
}
