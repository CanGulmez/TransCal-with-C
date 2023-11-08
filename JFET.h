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

// Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

// User-defined string type:
typedef char * string;

// Results of DC analysis:
struct DCResults {
   double Id; // drain current
   double Vgs; // gate-source voltage
   double Vds; // drain-source voltage
   double Vs; // source voltage
   double Vd; // drain voltage
   double Vg; // gate voltage
};

// Results of AC analysis:
struct ACResults {
   double gm; // transconductance factor
   double Zi; // input impedance
   double Zo; // output impedance
   double Av; // voltage gain
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
   printf("Id: %e A\n", analysis.Id);
   printf("Vgs: %f V\n", analysis.Vgs);
   printf("Vds: %f V\n", analysis.Vds);
   printf("Vg: %f V\n", analysis.Vg);
   printf("Vd: %f V\n", analysis.Vd);
   printf("Vs: %f V\n", analysis.Vs);
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

/* DC analysis of fixed-bias transistor configuration. 

double Vdd=16, Vgg=2, Rd=2000, Idss=0.01, Vp=-8;
DCAnalysis analysis = dc_fixed_bias(Vdd, Vgg, Rd, Idss, Vp);
display_dc_results(analysis);

Id: 5.625000e-03 A
Vgs: -2.000000 V
Vds: 4.750000 V
Vg: -2.000000 V
Vd: 4.750000 V
Vs: 0.000000 V
*/
DCAnalysis dc_fixed_bias(double Vdd, double Vgg, double Rd, 
                         double Idss, double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.Vgs = -1 * Vgg; 
   analysis.Id = Idss * (1.0 - analysis.Vgs / Vp) * 
                 (1.0 - analysis.Vgs / Vp);
   analysis.Vds = Vdd - analysis.Id * Rd;
   analysis.Vd = analysis.Vds; 
   analysis.Vg = analysis.Vgs; 
   analysis.Vs = 0; 

   return analysis;
}

/* AC analysis of fixed-bias transistor configuration. 

double Vdd=16, Vgg=2, Rd=2000, Rg=1e+6;
double Idss=0.01, Vp=-8, rd=25000;
ACAnalysis analysis = ac_fixed_bias(Vdd, Vgg, Rg, Rd, 
                                    Idss, Vp, rd);
display_ac_results(analysis);

gm: 1.875000e-03 S
Zi: 1000000.000000 ohm
Zo: 1851.851852 ohm
Av: -3.472222
Phase: Out of phase
*/
ACAnalysis ac_fixed_bias(double Vdd, double Vgg, double Rg, 
               double Rd, double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rg > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double Vgs = -1 * Vgg; 
   analysis.gm = _gm_factor_(Idss, Vp, Vgs);
   analysis.Zi = Rg; 
   analysis.Zo = _parallel_(Rd, rd); 
   analysis.Av = -1.0 * analysis.gm * analysis.Zo; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of self-bias transistor configuration. 

double Vdd=20, Rd=3300, Rs=1000, Idss=0.008, Vp=-6;
DCAnalysis analysis = dc_self_bias(Vdd, Rd, Rs, Idss, Vp);
display_dc_results(analysis);

Id: 2.587624e-03 A
Vgs: -2.587624 V
Vds: 8.873216 V
Vg: 0.000000 V
Vd: 11.460840 V
Vs: 2.587624 V
*/
DCAnalysis dc_self_bias(double Vdd, double Rd, double Rs, 
                        double Idss, double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using dicriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b = Idss * 2.0 * Rs / Vp - 1;
   double c = Idss;
   analysis.Id = _drain_current_(a, b, c); 
   analysis.Vgs = -1 * analysis.Id * Rs; 
   analysis.Vds = Vdd - analysis.Id * (Rs + Rd); 
   analysis.Vs = analysis.Id * Rs; 
   analysis.Vg = 0; 
   analysis.Vd = analysis.Vds + analysis.Vs; 

   return analysis;
}

/* AC analysis of self-bias transistor configuration. 

double Vdd=20, Rd=3300, Rs=1000, Vp=-6;
double Idss=0.008, Rg=1e+6, rd=50000;
ACAnalysis analysis = ac_self_bias(Vdd, Rg, Rd, Rs, 
                                   Idss, Vp, rd);
display_ac_results(analysis);

gm: 1.516611e-03 S
Zi: 1000000.000000 ohm
Zo: 3216.314824 ohm
Av: -1.922998
Phase: Out of phase
*/
ACAnalysis ac_self_bias(double Vdd, double Rg, double Rd, double Rs, 
                        double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rg > 0 && Rs > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor. For that, 
   // previously select the right Id using discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b = Idss * 2.0 * Rs / Vp - 1;
   double c = Idss;
   double Id = _drain_current_(a, b, c); 
   double Vgs = -1 * Id * Rs; 
   analysis.gm = _gm_factor_(Idss, Vp, Vgs);
   analysis.Zi = Rg; 
   double Zo1 = 1 + analysis.gm * Rs + Rs / rd;
   double Zo2 = 1 + analysis.gm * Rs + Rs / rd + Rd / rd;
   analysis.Zo = Zo1 * Rd / Zo2; 
   double Av1 = analysis.gm * Rd;
   double Av2 = 1.0 + analysis.gm * Rs + (Rd + Rs) / rd;
   analysis.Av = -1.0 * Av1 / Av2; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of voltage-divider transistor configuration. 

double Vdd=16, Rg1=21*1e+5, Rg2=27*1e+4;
double Rd=2400, Rs=1500, Idss=0.008, Vp=-4;
DCAnalysis analysis = dc_voltage_divider(Vdd, Rg1, Rg2, Rd, 
                                        Rs, Idss, Vp);
display_dc_results(analysis);

Id: 2.416309e-03 A
Vgs: -1.801678 V
Vds: 6.576396 V
Vg: 1.822785 V
Vd: 10.200859 V
Vs: 3.624463 V
*/
DCAnalysis dc_voltage_divider(double Vdd, double Rg1, double Rg2,
                  double Rd, double Rs, double Idss, double Vp){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b1 = (2.0 * Rs * Idss / Vp);
   double b2 = (2.0 * analysis.Vg * Rs * Idss / Vp / Vp);
   double b = b1 - b2 - 1;
   double c = Idss * (1.0 - (2.0 * analysis.Vg / Vp) + 
              (analysis.Vg * analysis.Vg / Vp / Vp));
   analysis.Id = _drain_current_(a, b, c); 
   analysis.Vgs = analysis.Vg - analysis.Id * Rs;  
   analysis.Vds = Vdd - analysis.Id * (Rs + Rd); 
   analysis.Vs = analysis.Id * Rs; 
   analysis.Vd = Vdd - analysis.Id * Rd; 

   return analysis;
}

/* AC analysis of voltage-divider transistor configuration. 

double Vdd=20, Rg1=82*1e+6, Rg2=11*1e+6, Rd=2000;
double Rs=610, Idss=0.012, Vp=-3, rd=5*1e+5;
ACAnalysis analysis = ac_voltage_divider(Vdd, Rg1, Rg2, Rd, 
                                         Rs, Idss, Vp, rd);
display_ac_results(analysis);

gm: 5.403363e-03 S
Zi: 9698924.731183 ohm
Zo: 1992.031873 ohm
Av: -10.763671
Phase: Out of phase
*/
ACAnalysis ac_voltage_divider(double Vdd, double Rg1, double Rg2,
         double Rd, double Rs, double Idss, double Vp, double rd){
   // Check if the parameters of transistor are consistent.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   double a = Rs * Rs * Idss / Vp / Vp;
   double b1 = (2.0 * Rs * Idss / Vp);
   double b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   double b = b1 - b2 - 1;
   double c = Idss * (1.0-(2.0 * Vg/Vp) + (Vg * Vg/Vp/Vp));
   double Id = _drain_current_(a, b, c); 
   double Vgs = Vg - Id * Rs;  
   analysis.gm = _gm_factor_(Idss, Vp, Vgs); 
   analysis.Zi = _parallel_(Rg1, Rg2); 
   analysis.Zo = _parallel_(Rd, rd); 
   analysis.Av = -1 * analysis.gm * analysis.Zo; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of common-gate transistor configuration. 

double Vdd=12, Vss=0, Rd=1500, Rs=680, Idss=0.012, Vp=-6;
DCAnalysis analysis = dc_common_gate(Vdd, Vss, Rd, Rs, Idss, Vp);
display_dc_results(analysis);

Id: 3.835265e-03 A
Vgs: -2.607980 V
Vds: 3.639121 V
Vg: 0.000000 V
Vd: 6.247102 V
Vs: 2.607980 V
*/
DCAnalysis dc_common_gate(double Vdd, double Vss, double Rd, 
                           double Rs, double Idss, double Vp) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // For quadritic equations, find discriminant.
   double a = (Rs * Rs) * Idss / (Vp * Vp);
   double b1 = 2.0 * Rs * Idss / Vp;
   double b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   double b = b1 + b2 - 1.0;
   double c1 = 2.0 * Vss / Vp;
   double c2 = Vss * Vss / Vp / Vp;
   double c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   analysis.Id = _drain_current_(a, b, c); 
   analysis.Vgs = Vss - analysis.Id * Rs; 
   analysis.Vds = Vdd + Vss - analysis.Id * (Rs + Rd);
   analysis.Vs = -Vss + analysis.Id * Rs; 
   analysis.Vd = Vdd - analysis.Id * Rd; 
   analysis.Vg = 0; 

   return analysis;
}

/* AC analysis of common-gate transistor configuration. 

double Vdd=15, Vss=0, Rd=3300, Rs=1500, Idss=0.008;
double Vp=-2.8, rd=4*1e+4;
ACAnalysis analysis = ac_common_gate(Vdd, Vss, Rd, Rs, 
                                     Idss, Vp, rd);
display_ac_results(analysis);

gm: 3.042147e-03 S
Zi: 285.709479 ohm
Zo: 3048.498845 ohm
Av: 9.350194
Phase: In phase
*/
ACAnalysis ac_common_gate(double Vdd, double Vss, double Rd, 
                  double Rs, double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rd > 0 && Rs > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // For quadritic equations, find discriminant.
   double a = (Rs * Rs) * Idss / (Vp * Vp);
   double b1 = 2.0 * Rs * Idss / Vp;
   double b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   double b = b1 + b2 - 1.0;
   double c1 = 2.0 * Vss / Vp;
   double c2 = Vss * Vss / Vp / Vp;
   double c = (1.0 - c1 + c2) * Idss;
   // Calculate the all analyzes of transistor.
   double Id = _drain_current_(a, b, c);
   double Vgs = Vss - Id * Rs; 
   analysis.gm = _gm_factor_(Idss, Vp, Vgs); 
   double Zi1 = (rd + Rd) / (1 + analysis.gm * rd);
   analysis.Zi = _parallel_(Rs, Zi1); 
   analysis.Zo = _parallel_(Rd, rd); 
   double Av1 = analysis.gm * Rd + Rd / rd;
   double Av2 = 1 + Rd / rd;
   analysis.Av = Av1 / Av2; 
   analysis.phase = "In phase";

   return analysis;
}

/* AC analysis of source-follower transistor configuration. 

double Vdd=9, Vgs=-2.86, Rg=1e+6, Rs=2200;
double Idss=0.016, Vp=-4, rd=4*1e+4; 
ACAnalysis analysis = ac_source_follower(Vdd, Vgs, Rg, Rs, 
                                         Idss, Vp, rd);
display_ac_results(analysis);

gm: 2.280000e-03 S
Zi: 1000000.000000 ohm
Zo: 362.378521 ohm
Av: 0.826223
Phase: In phase
*/
ACAnalysis ac_source_follower(double Vdd, double Vgs, double Rg,
                  double Rs, double Idss, double Vp, double rd) {
   // Check if the parameters of transistor are consistent.
   assert (Rg > 0 && Rs > 0 && rd > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.gm = _gm_factor_(Idss, Vp, Vgs); 
   analysis.Zi = Rg; 
   analysis.Zo = _parallel_(rd, _parallel_(Rs, 1/analysis.gm)); 
   double Av1 = analysis.gm * _parallel_(rd, Rs); 
   double Av2 = 1.0 + Av1;
   analysis.Av = Av1 / Av2; 
   analysis.phase = "In phase";

   return analysis;
}
