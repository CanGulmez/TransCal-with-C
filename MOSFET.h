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

1. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
2. All transistor configuratiions are set as 'npn' type. 

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
#include <assert.h>
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

/* Save the DC results of any kind of transistor. */
void save_dc_results(double k, double Id, double Vds, double Vgs){
   // Save the DC results into 'DCAnalysis' struct.
   DCAnalysis.k = k; // k constant
   DCAnalysis.Id = Id; // drain current
   DCAnalysis.Vds = Vds; // drain-source voltage
   DCAnalysis.Vgs = Vgs; // gate-source voltage
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

/* Display the DC results. */
void display_dc_results(void) {
   // Display the DC results.
   printf("k: %e A/V^2\n", DCAnalysis.k);
   printf("Id: %e A\n", DCAnalysis.Id);
   printf("Vgs: %f V\n", DCAnalysis.Vgs);
   printf("Vds: %f V\n", DCAnalysis.Vds);
}

/* Display the AC results. */
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

/* DC analysis of drain-feedback transistor configuration. 

'Vdd', 'Rg' and 'Rd' parameters stand for drain voltage, gate and
drain resistances. 'Idon' and 'Vgson' parameters are the threshold 
voltage and current level. 'Vgsth' parameter is the minimum value
to run the transistor. All resistances and currents must be in 
form of 'A' and 'ohm'. For example:

double Vdd=12, Rg=1e+7, Rd=2000, Idon=0.006, Vgson=8, Vgsth=3;
dc_drain_feedback(Vdd, Rg, Rd, Idon, Vgson, Vgsth);
display_dc_results();

k: 2.400000e-04 A/V^2
Id: 2.794004e-03 A
Vgs: 6.411991 V
Vds: 6.411991 V
*/
void dc_drain_feedback(double Vdd,double Rg,double Rd,double Idon, 
                       double Vgson, double Vgsth) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rd > 0);

   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   double a = Rd * Rd * k;
   double b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   double c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vdd - Id * Rd; // gate-source voltage
   double Vds = Vgs; // drain-source voltage

   // Save and display the DC results of transistor.
   save_dc_results(k, Id, Vds, Vgs);
}

/* AC analysis of drain-feedback transistor configuration. 

'Vdd', 'Rg' and 'Rd' parameters stand for drain voltage, gate and
drain resistances. 'Idon' and 'Vgson' parameters are the threshold 
voltage and current level. 'Vgsth' parameter is the minimum value
to run the transistor. 'rd' parameter is the drain resistance. All 
resistances and currents must be in form of 'A' and 'ohm'. For
example:

double Vdd=12, Rg=1e+7, Rd=2000, Idon=0.006;
double Vgson=8, Vgsth=3, rd=5*1e+4;
ac_drain_feedback(Vdd, Rg, Rd, Idon, Vgson, Vgsth, rd);
display_ac_results();

gm: 1.637756e-03 S
Zi: 2410374.750000 ohm
Zo: 1922.707153 ohm
Av: -3.148925
Phase: Out of phase
*/
void ac_drain_feedback(double Vdd,double Rg,double Rd,double Idon, 
                       double Vgson, double Vgsth, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rd > 0 && rd > 0);

   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   double a = Rd * Rd * k;
   double b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   double c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vdd - Id * Rd; // gate-source voltage
   double gm = 2 * k * (Vgs - Vgsth); // gm factor
   double Zi1 = Rg + parallel(rd, Rd);
   double Zi2 = 1 + gm * parallel(rd, Rd); 
   double Zi = Zi1 / Zi2; // input impedance
   double Zo1 = parallel(rd, Rd);
   double Zo = parallel(Rg, Zo1); // output impedance
   double Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd' and 'Rs' parameters stand for drain 
voltage, upper gate resistance, lower gate resistance, drain
resistance, source resistance in order. 'Idon' and 'Vgson' 
parameters are the threshold voltage and current level. 'Vgsth' 
parameter is the minimum value to run the transistor. All 
resistances and currents must be in form of 'A' and 'ohm'. For
example:

double Vdd=40, Rg1=22*1e+6, Rg2=18*1e+6, Rd=3000;
double Rs=820, Idon=0.003, Vgson=10, Vgsth=5;
dc_voltage_divider(Vdd, Rg1, Rg2, Rd, Rs, Idon, Vgson, Vgsth);
display_dc_results();

k: 1.200000e-04 A/V^2
Id: 6.724565e-03 A
Vgs: 12.485856 V
Vds: 14.312160 V
*/
void dc_voltage_divider(double Vdd,double Rg1,double Rg2,double Rd, 
                  double Rs,double Idon,double Vgson,double Vgsth){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);

   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   double Vg = Rg2 * Vdd / (Rg1 + Rg2);
   double a = Rs * Rs * k;
   double b = -2 * k * Rs * (Vg - Vgsth) - 1;
   double c = k * (Vgsth - Vg) * (Vgsth - Vg);
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vg - Id * Rs; // gate-source voltage
   double Vds = Vdd - Id * (Rs + Rd); // drain-source voltage

   // Save and display the DC results of transistor.
   save_dc_results(k, Id, Vds, Vgs);
}

/* AC analysis of voltage-divider transistor configuration. 

'Vdd', 'Rg1', 'Rg2', 'Rd' and 'Rs' parameters stand for drain 
voltage, upper gate resistance, lower gate resistance, drain
resistance, source resistance in order. 'Idon' and 'Vgson' 
parameters are the threshold voltage and current level. 'Vgsth' 
parameter is the minimum value to run the transistor. 'rd' 
parameter is the drain resistance. All resistances and currents 
must be in form of 'A' and 'ohm'. For example:

double Vdd=24, Rg1=1e+7, Rg2=6.8*1e+6, Rd=2200;
double Rs=750, Idon=0.005, Vgson=6, Vgsth=3, rd=1e+6;
ac_voltage_divider(Vdd, Rg1, Rg2, Rd, Rs, Idon, Vgson, Vgsth, rd);
display_ac_results();

gm: 3.321982e-03 S
Zi: 4047619.000000 ohm
Zo: 2195.170654 ohm
Av: -7.292316
Phase: Out of phase
*/
void ac_voltage_divider(double Vdd,double Rg1,double Rg2,double Rd, 
         double Rs,double Idon,double Vgson,double Vgsth,double rd){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0 && rd > 0);

   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   double Vg = Rg2 * Vdd / (Rg1 + Rg2);
   double a = Rs * Rs * k;
   double b = -2 * k * Rs * (Vg - Vgsth) - 1;
   double c = k * (Vgsth - Vg) * (Vgsth - Vg);
   double Id = select_right_Id(a, b, c); // drain current
   double Vgs = Vg - Id * Rs; // gate-source voltage
   double gm = 2 * k * (Vgs - Vgsth); // gm factor
   double Zi = parallel(Rg1, Rg2); // input impedance
   double Zo = parallel(rd, Rd); // output impedance
   double Av = -1 * gm * Zo; // voltage gain

   // Save and display the AC results of transistor.
   save_ac_results(gm, Zi, Zo, Av, "Out of phase");
}
