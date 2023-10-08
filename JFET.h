/* The DC and AC Analysis of JFET Transistor Configurations

JFET transistor have a large usage area in electronics and related
discipline. So, I've written this source file which contain mostly
used JFET transistor calculations. 

Before using this source file, please, read these important notes. 
I hope, it will be usefull...

IMPORTANT NOTES:
----------------

1. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
2. All transistor configuratiions are set as 'npn' type. 
3. In ac analysis, algorithms use 'JFET small signal' model.

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
#include <assert.h>
#include <math.h>
#include <string.h>

// Results of DC analysis
struct DCResults {
   double Id; // drain current
   double Vgs; // gate-source voltage
   double Vds; // drain-source voltage
   double Vs; // source voltage
   double Vd; // drain voltage
   double Vg; // gate voltage
} DCAnalysis;

// Results of AC analysis
struct ACResults {
   double gm; // transconductance factor
   double Zi; // input impedance
   double Zo; // output impedance
   double Av; // voltage gain
   char *phase; // phase relationship
} ACAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

/* Save the RC results of any kind of transistor. */
void save_dc_results(double Id, double Vds, double Vgs, double Vs, 
                     double Vd, double Vg) {
   // Save the DC results into 'DCAnalysis' struct.
   DCAnalysis.Id = Id; // drain current
   DCAnalysis.Vds = Vds; // drain-source voltage
   DCAnalysis.Vgs = Vgs; // gate-source voltage
   DCAnalysis.Vs = Vs; // source voltage
   DCAnalysis.Vd = Vd; // drain voltage
   DCAnalysis.Vg = Vg; // gate voltage
}

/* Save the AC results of any kind of transistor. */
void save_ac_results(double gm, double Zi, double Zo, double Av, 
                     char *phase) {
   // Save the AC results into 'ACAnalysis' struct.
   ACAnalysis.gm = gm; // transconductance factor
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationship
}

/* Display the DC results of any kind of transistor. */
void display_dc_results(void) {
   // Display the DC results.
   printf("Id: %e A\n", DCAnalysis.Id);
   printf("Vgs: %f V\n", DCAnalysis.Vgs);
   printf("Vds: %f V\n", DCAnalysis.Vds);
   printf("Vg: %f V\n", DCAnalysis.Vg);
   printf("Vd: %f V\n", DCAnalysis.Vd);
   printf("Vs: %f V\n", DCAnalysis.Vs);
}

/* Display the AC results of any kind of transistor. */
void display_ac_results(void) {
   // Display the AC results.
   printf("gm: %e S\n", ACAnalysis.gm);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("Phase: %s\n", ACAnalysis.phase);
}

/* Get the parallel resultant of 'R1' and 'R2'. */
double parallel(double R1, double R2) {
   // Find the resultant resistance for parallel resistors.
   return 1.0 / (1.0 / R1 + 1.0 / R2);
}

/* Find the transconductance factor (gm). */
double find_gm_factor(double Idss, double Vp, double Vgs) {
   // Find the transconductance factor (gm).
   return (2.0 * Idss / abs(Vp)) * (1.0 - Vgs / Vp);
}

/* Select the right drain current using discriminant. */
double select_right_Id(double a, double b, double c) {
   // Find the discriminant and calculate two different roots.
   double dicriminant = (b * b) - (4 * a * c);
   double root1 = (-1.0 * b + sqrt(dicriminant)) / (2 * a);
   double root2 = (-1.0 * b - sqrt(dicriminant)) / (2 * a);
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

/* --------------------------------------------------------------- */
/* ------------------------ Main Definations --------------------- */
/* --------------------------------------------------------------- */

/* DC analysis of fixed-bias transistor configuration. 

'Vdd', 'Vgg', 'Rg' and 'Rd' parameters stand for drain voltage,
gate voltage, gate resistance and drain resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. All current and resistance values must be in 
form of 'A' and 'ohm'. For example:

double Vdd=16, Vgg=2, Rd=2000, Idss=0.01, Vp=-8;
dc_fixed_bias(Vdd, Vgg, Rd, Idss, Vp);
display_dc_results();

Id: 5.625000e-03 A
Vgs: -2.000000 V
Vds: 4.750000 V
Vg: -2.000000 V
Vd: 4.750000 V
Vs: 0.000000 V
*/
void dc_fixed_bias(double Vdd, double Vgg, double Rd, double Idss,
                   double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0);

   // Calculate the all analyzes of transistor.
   double Vgs = -1 * Vgg; // gate-source voltage
   double Id = Idss * (1.0-Vgs/Vp) * (1.0-Vgs/Vp);// drain current
   double Vds = Vdd - Id * Rd; // drain-source voltage
   double Vd = Vds; // drain voltage
   double Vg = Vgs; // gate voltage
   double Vs = 0; // source voltage

   // Save and display the DC results of transistor.
   save_dc_results(Id, Vds, Vgs, Vs, Vd, Vg);
}

/* AC analysis of fixed-bias transistor configuration. 

'Vdd', 'Vgg', 'Rg' and 'Rd' parameters stand for drain voltage,
gate voltage, gate resistance and drain resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All 
current and resistance values must be in form of 'A' and 'ohm'.
For example:

double Vdd=16, Vgg=2, Rd=2000, Rg=1e+6;
double Idss=0.01, Vp=-8, rd=25000;
ac_fixed_bias(Vdd, Vgg, Rg, Rd, Idss, Vp, rd);
display_ac_results();

gm: 1.875000e-03 S
Zi: 1000000.000000 ohm
Zo: 1851.851852 ohm
Av: -3.472222
Phase: Out of phase
*/
void ac_fixed_bias(double Vdd, double Vgg, double Rg, double Rd, 
                   double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rg > 0 && rd > 0);

   // Calculate the all analyzes of transistor.
   double Vgs = -1 * Vgg; // gate-source voltage
   double gm = find_gm_factor(Idss, Vp, Vgs); // gm
   double Zi = Rg; // input impedance
   double Zo = parallel(Rd, rd); // output impedance
   double Av = -1.0 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of self-bias transistor configuration. 

'Vdd', 'Rg', 'Rd', 'Rs' parameters stand for drain voltage, gate
resistance, drain resistance and source resistance in order. 'Idss'
parameter is the maximum drain current. 'Vp' parameter is the
pinch-off voltage. All current and resistance values must be in 
form of 'A' and 'ohm'. For example:

double Vdd=20, Rd=3300, Rs=1000, Idss=0.008, Vp=-6;
dc_self_bias(Vdd, Rd, Rs, Idss, Vp);
display_dc_results();

Id: 2.587624e-03 A
Vgs: -2.587624 V
Vds: 8.873216 V
Vg: 0.000000 V
Vd: 11.460840 V
Vs: 2.587624 V
*/
void dc_self_bias(double Vdd, double Rd, double Rs, double Idss,
                  double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0);

   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using dicriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b = Idss * 2.0 * Rs / Vp - 1;
   double c = Idss;
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = -1 * Id * Rs; // gate-source voltage
   double Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   double Vs = Id * Rs; // source voltage
   double Vg = 0; // gate voltage
   double Vd = Vds + Vs; // drain voltage

   // Save and display the DC results of transistor.
   save_dc_results(Id, Vds, Vgs, Vs, Vd, Vg);
}

/* AC analysis of self-bias transistor configuration. 

'Vdd', 'Rg', 'Rd', 'Rs' parameters stand for drain voltage, gate
resistance, drain resistance and source resistance in order. 'Idss'
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All
current and resistance values must be in form of 'A' and 'ohm'.
For example:

double Vdd=20, Rd=3300, Rs=1000, Vp=-6;
double Idss=0.008, Rg=1e+6, rd=50000;
ac_self_bias(Vdd, Rg, Rd, Rs, Idss, Vp, rd);
display_ac_results();

gm: 1.516611e-03 S
Zi: 1000000.000000 ohm
Zo: 3216.314824 ohm
Av: -1.922998
Phase: Out of phase
*/
void ac_self_bias(double Vdd, double Rg, double Rd, double Rs, 
                  double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rg > 0 && Rs > 0 && rd > 0);

   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b = Idss * 2.0 * Rs / Vp - 1;
   double c = Idss;
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = -1 * Id * Rs; // gate-source voltage
   double gm = find_gm_factor(Idss, Vp, Vgs); // gm factor
   double Zi = Rg; // input impedance
   double Zo1 = 1 + gm * Rs + Rs / rd;
   double Zo2 = 1 + gm * Rs + Rs / rd + Rd / rd;
   double Zo = Zo1 * Rd / Zo2; // output impedance
   double Av1 = gm * Rd;
   double Av2 = 1.0 + gm * Rs + (Rd + Rs) / rd;
   double Av = -1.0 * Av1 / Av2; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd', 'Rs' parameters stand for drain voltage,
upper gate resistance, lower gate resistance, drain resistance and
source resistance. 'Idss' parameter is the maximum drain current. 
'Vp' parameter is the pinch-off voltage. All current and resistance 
values must be in form of 'A' and 'ohm'. For example:

double Vdd=16, Rg1=21*1e+5, Rg2=27*1e+4;
double Rd=2400, Rs=1500, Idss=0.008, Vp=-4;
dc_voltage_divider(Vdd, Rg1, Rg2, Rd, Rs, Idss, Vp);
display_dc_results();

Id: 2.416309e-03 A
Vgs: -1.801678 V
Vds: 6.576396 V
Vg: 1.822785 V
Vd: 10.200859 V
Vs: 3.624463 V
*/
void dc_voltage_divider(double Vdd,double Rg1,double Rg2,double Rd, 
                        double Rs, double Idss, double Vp){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);

   // Calculate the all analyzes of transistor.
   double Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b1 = (2.0 * Rs * Idss / Vp);
   double b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   double b = b1 - b2 - 1;
   double c = Idss * (1.0-(2.0 * Vg/Vp) + (Vg * Vg/Vp/Vp));
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vg - Id * Rs;  // gate-source voltage
   double Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   double Vs = Id * Rs; // source voltage
   double Vd = Vdd - Id * Rd; // drain voltage

   // Save and display the DC results of transistor.
   save_dc_results(Id, Vds, Vgs, Vs, Vd, Vg);
}

/* AC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd', 'Rs' parameters stand for drain voltage,
upper gate resistance, lower gate resistance, drain resistance and
source resistance. 'Idss' parameter is the maximum drain current. 
'Vp' parameter is the pinch-off voltage. 'rd' parameter is the drain 
resistance. All current and resistance values must be in form 
of 'A' and 'ohm'. For example:

double Vdd=20, Rg1=82*1e+6, Rg2=11*1e+6, Rd=2000;
double Rs=610, Idss=0.012, Vp=-3, rd=5*1e+5;
ac_voltage_divider(Vdd, Rg1, Rg2, Rd, Rs, Idss, Vp, rd);
display_ac_results();

gm: 5.403363e-03 S
Zi: 9698924.731183 ohm
Zo: 1992.031873 ohm
Av: -10.763671
Phase: Out of phase
*/
void ac_voltage_divider(double Vdd,double Rg1,double Rg2,double Rd,
                        double Rs,double Idss,double Vp,double rd){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);

   // Calculate the all analyzes of transistor.
   double Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b1 = (2.0 * Rs * Idss / Vp);
   double b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   double b = b1 - b2 - 1;
   double c = Idss * (1.0-(2.0 * Vg/Vp) + (Vg * Vg/Vp/Vp));
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vg - Id * Rs;  // gate-source voltage
   double gm = find_gm_factor(Idss, Vp, Vgs); // gm factor
   double Zi = parallel(Rg1, Rg2); // input impedance
   double Zo = parallel(Rd, rd); // output impedance
   double Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of common-gate transistor configuration. 

'Vdd', 'Vss', 'Rd', 'Rs' parameters stand for drain voltage, 
source voltage, drain resistance and source resistance. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. All current and resistance must be in form 
of 'A' and 'ohm'. For example:

double Vdd=12, Vss=0, Rd=1500, Rs=680, Idss=0.012, Vp=-6;
dc_common_gate(Vdd, Vss, Rd, Rs, Idss, Vp);
display_dc_results();

Id: 3.835265e-03 A
Vgs: -2.607980 V
Vds: 3.639121 V
Vg: 0.000000 V
Vd: 6.247102 V
Vs: 2.607980 V
*/
void dc_common_gate(double Vdd, double Vss, double Rd, double Rs, 
                    double Idss, double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0);

   // For quadritic equations, find discriminant.
   double a = (Rs * Rs) * Idss / (Vp * Vp);
   double b1 = 2.0 * Rs * Idss / Vp;
   double b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   double b = b1 + b2 - 1.0;
   double c1 = 2.0 * Vss / Vp;
   double c2 = Vss * Vss / Vp / Vp;
   double c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vss - Id * Rs; // gate-source voltage
   double Vds = Vdd + Vss - Id * (Rs + Rd);// drain-source voltage
   double Vs = -Vss + Id * Rs; // source voltage
   double Vd = Vdd - Id * Rd; // drain voltage
   double Vg = 0; // gate voltage

   // Save and display the DC results of transistor.
   save_dc_results(Id, Vds, Vgs, Vs, Vd, Vg);
}

/* AC analysis of common-gate transistor configuration. 

'Vdd', 'Vss', 'Rd', 'Rs' parameters stand for drain voltage, 
source voltage, drain resistance and source resistance. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' parameter is the drain resistance. All 
current and resistance must be in form of 'A' and 'ohm'. 
For example: 

double Vdd=15, Vss=0, Rd=3300, Rs=1500, Idss=0.008;
double Vp=-2.8, rd=4*1e+4;
ac_common_gate(Vdd, Vss, Rd, Rs, Idss, Vp, rd);
display_ac_results();

gm: 3.042147e-03 S
Zi: 285.709479 ohm
Zo: 3048.498845 ohm
Av: 9.350194
Phase: In phase
*/
void ac_common_gate(double Vdd, double Vss, double Rd, double Rs, 
                    double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0 && rd > 0);

   // For quadritic equations, find discriminant.
   double a = (Rs * Rs) * Idss / (Vp * Vp);
   double b1 = 2.0 * Rs * Idss / Vp;
   double b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   double b = b1 + b2 - 1.0;
   double c1 = 2.0 * Vss / Vp;
   double c2 = Vss * Vss / Vp / Vp;
   double c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vss - Id * Rs; // gate-source voltage
   double gm = find_gm_factor(Idss, Vp, Vgs); // gm factor
   double Zi1 = (rd + Rd) / (1 + gm * rd);
   double Zi = parallel(Rs, Zi1); // input impedance
   double Zo = parallel(Rd, rd); // output impedance
   double Av1 = gm * Rd + Rd / rd;
   double Av2 = 1 + Rd / rd;
   double Av = Av1 / Av2; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "In phase");
}

/* AC analysis of source-follower transistor configuration. 

'Vdd', 'Vgs', 'Rg', 'Rs' parameters stand for drain voltage, gate-
source voltage, gate resistance, source resistance in order. 'Idss' 
parameter is the maximum drain current. 'Vp' parameter is the 
pinch-off voltage. 'rd' paramter is the drain resistance. For 
example:

double Vdd=9, Vgs=-2.86, Rg=1e+6, Rs=2200;
double Idss=0.016, Vp=-4, rd=4*1e+4; 
ac_source_follower(Vdd, Vgs, Rg, Rs, Idss, Vp, rd);
display_ac_results();

gm: 2.280000e-03 S
Zi: 1000000.000000 ohm
Zo: 362.378521 ohm
Av: 0.826223
Phase: In phase
*/
void ac_source_follower(double Vdd,double Vgs,double Rg,double Rs, 
                        double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rs > 0 && rd > 0);

   // Calculate the all analyzes of transistor.
   double gm = find_gm_factor(Idss, Vp, Vgs); // gm factor
   double Zi = Rg; // input impedance
   double Zo = parallel(rd, parallel(Rs, 1/gm)); // output impedence
   double Av1 = gm * parallel(rd, Rs); 
   double Av2 = 1.0 + Av1;
   double Av = Av1 / Av2; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "In phase");
}
