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

// Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

// User-defined string type:
typedef char * string;

// Results of DC analysis:
struct DCResults {
   float k; // k constant
   float Id; // drain current
   float Vgs; // gate-source voltage
   float Vds; // drain-gate voltage
};

// Results of AC analysis:
struct ACResults {
   float gm; // transconductance factor
   float Zi; // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   string phase; // phase relationship
};

// User-defined analysis types:
typedef struct DCResults DCAnalysis;
typedef struct ACResults ACAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

/* Get the parallel resultant of 'R1' and 'R2'. */
double _parallel_(double R1, double R2) {
   // Find the resultant resistance for parallel resistors.
   return 1.0 / (1.0 / R1 + 1.0 / R2);
}

/* Find the transconductance factor (gm). */
double _gm_factor_(double Idss, double Vp, double Vgs) {
   // Find the transconductance factor (gm).
   return (2.0 * Idss / abs(Vp)) * (1.0 - Vgs / Vp);
}

/* Select the right drain current using discriminant. */
double _drain_current_(double a, double b, double c) {
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
/* ------------------------- Display Results --------------------- */
/* --------------------------------------------------------------- */

/* Display the DC results of any kind of transistor. */
void display_dc_results(DCAnalysis analysis) {
   // Display the DC results.
   printf("k: %e A/V^2\n", analysis.k);
   printf("Id: %e A\n", analysis.Id);
   printf("Vgs: %f V\n", analysis.Vgs);
   printf("Vds: %f V\n", analysis.Vds);
}

/* Display the AC results of any kind of transistor. */
void display_ac_results(ACAnalysis analysis) {
   // Display the AC results.
   printf("gm: %e S\n", analysis.gm);
   printf("Zi: %f ohm\n", analysis.Zi);
   printf("Zo: %f ohm\n", analysis.Zo);
   printf("Av: %f\n", analysis.Av);
   printf("Phase: %s\n", analysis.phase);
}

/* --------------------------------------------------------------- */
/* ------------------------ Main Definations --------------------- */
/* --------------------------------------------------------------- */

/* DC analysis of drain-feedback transistor configuration. 

double Vdd=12, Rg=1e+7, Rd=2000, Idon=0.006, Vgson=8, Vgsth=3;
DCAnalysis analysis = dc_drain_feedback(Vdd, Rg, Rd, Idon, 
                                        Vgson, Vgsth);
display_dc_results(analysis);

k: 2.400000e-04 A/V^2
Id: 2.794004e-03 A
Vgs: 6.411991 V
Vds: 6.411991 V
*/
DCAnalysis dc_drain_feedback(double Vdd, double Rg, double Rd, 
                        double Idon, double Vgson, double Vgsth) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rd > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   double a = Rd * Rd * analysis.k;
   double b = 2 * analysis.k * Rd * (Vgsth - Vdd) - 1;
   double c = analysis.k * (Vgsth -Vdd) * (Vgsth - Vdd);
   analysis.Id = _drain_current_(a, b, c); 
   analysis.Vgs = Vdd - analysis.Id * Rd; 
   analysis.Vds = analysis.Vgs; 

   return analysis;
}

/* AC analysis of drain-feedback transistor configuration. 

double Vdd=12, Rg=1e+7, Rd=2000, Idon=0.006;
double Vgson=8, Vgsth=3, rd=5*1e+4;
ACAnaysis analysis = ac_drain_feedback(Vdd, Rg, Rd, Idon, 
                                       Vgson, Vgsth, rd);
display_ac_results(analysis);

gm: 1.637756e-03 S
Zi: 2410374.750000 ohm
Zo: 1922.707153 ohm
Av: -3.148925
Phase: Out of phase
*/
ACAnalysis ac_drain_feedback(double Vdd, double Rg, double Rd, 
            double Idon, double Vgson, double Vgsth, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rd > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth)); 
   double a = Rd * Rd * k;
   double b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   double c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   double Id = _drain_current_(a, b, c); 
   double Vgs = Vdd - Id * Rd; 
   analysis.gm = 2 * k * (Vgs - Vgsth); 
   double Zi1 = Rg + _parallel_(rd, Rd);
   double Zi2 = 1 + analysis.gm * _parallel_(rd, Rd); 
   analysis.Zi = Zi1 / Zi2; 
   double Zo1 = _parallel_(rd, Rd);
   analysis.Zo = _parallel_(Rg, Zo1); 
   analysis.Av = -1 * analysis.gm * analysis.Zo; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of voltage-divider transistor configuration. 

double Vdd=40, Rg1=22*1e+6, Rg2=18*1e+6, Rd=3000;
double Rs=820, Idon=0.003, Vgson=10, Vgsth=5;
DCAnalysis analysis = dc_voltage_divider(Vdd, Rg1, Rg2, Rd, 
                                    Rs, Idon, Vgson, Vgsth);
display_dc_results(analysis);

k: 1.200000e-04 A/V^2
Id: 6.724565e-03 A
Vgs: 12.485856 V
Vds: 14.312160 V
*/
DCAnalysis dc_voltage_divider(double Vdd, double Rg1, double Rg2,
   double Rd, double Rs, double Idon, double Vgson, double Vgsth){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   double Vg = Rg2 * Vdd / (Rg1 + Rg2);
   double a = Rs * Rs * analysis.k;
   double b = -2 * analysis.k * Rs * (Vg - Vgsth) - 1;
   double c = analysis.k * (Vgsth - Vg) * (Vgsth - Vg);
   analysis.Id = _drain_current_(a, b, c); 
   analysis.Vgs = Vg - analysis.Id * Rs; 
   analysis.Vds = Vdd - analysis.Id * (Rs + Rd); 

   return analysis;
}

/* AC analysis of voltage-divider transistor configuration. 

double Vdd=24, Rg1=1e+7, Rg2=6.8*1e+6, Rd=2200;
double Rs=750, Idon=0.005, Vgson=6, Vgsth=3, rd=1e+6;
ACAnalysis analysis = ac_voltage_divider(Vdd, Rg1, Rg2, Rd, 
                                 Rs, Idon, Vgson, Vgsth, rd);
display_ac_results(analysis);

gm: 3.321982e-03 S
Zi: 4047619.000000 ohm
Zo: 2195.170654 ohm
Av: -7.292316
Phase: Out of phase
*/
ACAnalysis ac_voltage_divider(double Vdd, double Rg1, double Rg2,
   double Rd, double Rs, double Idon, double Vgson, double Vgsth,
   double rd){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   double Vg = Rg2 * Vdd / (Rg1 + Rg2);
   double a = Rs * Rs * k;
   double b = -2 * k * Rs * (Vg - Vgsth) - 1;
   double c = k * (Vgsth - Vg) * (Vgsth - Vg);
   double Id = _drain_current_(a, b, c); 
   double Vgs = Vg - Id * Rs; 
   analysis.gm = 2 * k * (Vgs - Vgsth); 
   analysis.Zi = _parallel_(Rg1, Rg2); 
   analysis.Zo = _parallel_(rd, Rd); 
   analysis.Av = -1 * analysis.gm * analysis.Zo;
   analysis.phase = "Out of phase"; 

   return analysis;
}
