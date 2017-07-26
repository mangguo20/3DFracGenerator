/*
 * Output_off_vtu_gmv.hh
 *
 *  Created on: 25 août 2016
 *      Author: ngotr
 */


/*
 * This function is to export surface mesh sm to *.off and *.vtu output
 * outputName	: 	output name
 * sm			:	surface mesh
 * off_out		:	true if *.off is set for output, false if not
 * vtu_out		:	true if *.vtu is set for output, false if not
 * gmv_out		:	true if *.gmv is set for output, false if not
 *
 */

#ifndef OUTPUT_OFF_VTU_GMV_HH_
#define OUTPUT_OFF_VTU_GMV_HH_

#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <sstream>

#include "read_off_write_vtk_xml_file.h"
#include "off_to_gmv.h"

namespace CGAL{

template <typename SM_>
void output_off_vtu_gmv(std::string outputName, SM_ &sm, bool off_out, bool vtu_out, bool gmp_out){

	// Write mesh to *off file
	std::string  off_file(outputName + ".off");
	std::ofstream os(off_file.c_str()) ; os << sm ;
	os.close();

	// Write mesh to vtu file
	if (vtu_out){
	CGAL::read_off_write_vtk_xml_file(outputName);}

	if (gmp_out){
	CGAL::off_to_gmv(outputName);}

	if (!off_out){
		remove(off_file.c_str());}
}

}

#endif /* OUTPUT_OFF_VTU_GMV_HH_ */
