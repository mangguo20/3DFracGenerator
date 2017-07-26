/*
 * plane_plane_intersection.hh
 *
 *  Created on: 9 août 2016
 *      Author: ngotr
 */

#ifndef PLANE_PLANE_INTERSECTION_CONVERT2MP_HH_
#define PLANE_PLANE_INTERSECTION_CONVERT2MP_HH_

#include <CGAL/utils.h>
#include <CGAL/number_utils.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Kernel/global_functions.h>
#include <list>
#include "list_seg_index_unique.hh"
#include "convert_point_numtype.hh"

#include "Exact_predicates_inexact_constructions_kernel_ntd_MP_Float.h"
//#include "include/Exact_predicates_inexact_constructions_kernel_ntd_float.h"

#define VERBOSE 1

namespace CGAL {

/*
template <typename PT_, typename PT_MP>
PT_MP NT_to_NT_MP(const PT_ & hp)
{
	return PT_MP (hp.hx(), hp.hy(), hp.hz(), hp.hw() );
}

template <typename PT_MP, typename PT_>
PT_ NT_MP_to_NT(const PT_MP & hp)
{
	return PT_ (hp.hx(), hp.hy(), hp.hz(), hp.hw() );
}
 */

template <typename Surface_mesh, typename Output_IP>
Output_IP intersections_of_two_polys_mp (Surface_mesh &polygon1, Surface_mesh &polygon2){

	typedef CGAL::Exact_predicates_inexact_constructions_kernel_MP_Float Kernel_MP;
	typedef Kernel_MP::Point_3                    Point_3_;
	typedef Kernel_MP::Vector_3                   Vector_3_;
	typedef Kernel_MP::Line_3                     Line_3_;
	typedef Kernel_MP::Segment_3                  Segment_3_;
	typedef Kernel_MP::Plane_3                    Plane_3_;

	typedef typename Surface_mesh::Vertex_index 		   vertex_descriptor;

	Output_IP output_;

	Plane_3_ pl_polygon1,pl_polygon2; // Planes which pass through the polygon1 and polygon2
	Point_3_ pt_1_polygon1, pt_2_polygon1, pt_3_polygon1; 		// Choose 3 point to define the plane of polygon1
	Point_3_ pt_1_polygon2, pt_2_polygon2, pt_3_polygon2; 	// Choose 3 point to define the plane of polygon2

	int num_intersec_1(0), num_intersec_2(0);
	std::list<Point_3_> intersection_points_1, intersection_points_2;
	std::list<Kernel::Point_3> intersection_points_1_out, intersection_points_2_out;
	std::list<int> list_seg_interst1, list_seg_interst2;

	bool no_polygons_interst(false), point_interst(false), point_interst2(false);
	std::list<bool> no_ls_interst_vec1, no_ls_interst_vec2;

	{
		unsigned int i = 0, end = polygon1.number_of_vertices();
		for( ; i < end; ++i) {
			vertex_descriptor vh(i);
			//if (i==polygon1.num_vertices/2)
			if (i==0)
				pt_1_polygon1 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon1.point(vh) );
			else
				if(i==round(polygon1.num_vertices()/4))
					pt_2_polygon1 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon1.point(vh) );
				else
					if (i==round(polygon1.num_vertices()/2))
						pt_3_polygon1 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon1.point(vh) );
			//std::cout << polygon1.point(vh) << std::endl;
		}
	}

	{
		unsigned int i = 0, end = polygon2.number_of_vertices();
		for( ; i < end; ++i) {
			vertex_descriptor vh(i);
			if (i==0)
				pt_1_polygon2 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon2.point(vh) );
			else if(i==round(polygon2.num_vertices()/4))
				pt_2_polygon2 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon2.point(vh) );
			else if (i==(polygon2.num_vertices()/2))
				pt_3_polygon2 = CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon2.point(vh) );
		}
	}

	pl_polygon1 = Plane_3_(pt_1_polygon1, pt_2_polygon1, pt_3_polygon1);
	pl_polygon2 = Plane_3_(pt_1_polygon2, pt_2_polygon2, pt_3_polygon2);

	if (VERBOSE == 1){
		std::cout << "Plane 1 : " << pl_polygon1 << std::endl;
		std::cout << "Plane 2 : " << pl_polygon2 << std::endl;
	}

	// ###########################################################################
	// Check if two planes intersect
	// ###########################################################################

	CGAL::cpp11::result_of<Kernel_MP::Intersect_3(Plane_3_, Plane_3_)>::type result = CGAL::intersection(pl_polygon1, pl_polygon2);

	Line_3_ Line_Int;

	if (!result) {
		if (VERBOSE == 1){
			std::cout << "Two planes are parallel" << std::endl;
		}
	}
	else{
		//if (const Line_3_* li = boost::get<Line_3_>(&*result)) {
		if (const Line_3_* li = boost::get<Line_3_>(&*result)) {
			//std::cout << *s << std::endl;}
			if (VERBOSE == 1){
				std::cout << "Two planes intersect in a line" << std::endl;
				std::cout << *li << std::endl;
			}
			Line_Int = *li;


			// ###########################################################################
			// Check if plane-plane intersection intersect the polygons
			// ###########################################################################

			// Ellipse 1
			if (VERBOSE == 1){
				std::cout << "###########################" << std::endl;
			}

			{
				unsigned int i = 0, end = polygon1.number_of_vertices();
				for( ; i < end; ++i) {
					vertex_descriptor vb_seg1(i);
					vertex_descriptor ve_seg1(i+1);
					vertex_descriptor ve_seg1_0(0);

					if (i==end-1)
						ve_seg1 = ve_seg1_0;

					Segment_3_ mesh_1_seg = Segment_3_(CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon1.point(vb_seg1)), CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon1.point(ve_seg1)));
					CGAL::cpp11::result_of<Kernel_MP::Intersect_3(Segment_3_, Line_3_)>::type result_sl = CGAL::intersection(mesh_1_seg, Line_Int);

					if (result_sl) {
						if (const Segment_3_* s = boost::get<Segment_3_>(&*result_sl)) {
							if (VERBOSE == 1){
								std::cout << "The segment lies on the plane-plane intersection." << std::endl;
								//std::cout << *s << std::endl;
							}
						}
						else
							if (const Point_3_* p = boost::get<Point_3_ >(&*result_sl)) {
								//std::cout << "The segment and the plane-plane intersection intersect on a point." << std::endl;
								//std::cout << *p << std::endl;

								if (*p == polygon1.point(ve_seg1)){
									if (VERBOSE == 1){
										std::cout << "The segment and the plane-plane intersection intersect on the vertex linking the segment " << i
												<< " and the segment "<< i+1 << std::endl;
										std::cout << "Choose the segment " << i+1  << " as the segment of intersection."<< std::endl;
									}
								}
								else{
									if (VERBOSE == 1){
										std::cout << "The segment and the plane-plane intersection intersect on a point: "<<
												*p << " on the segment " << i << std::endl;
									}
									point_interst =true;
									list_seg_interst1.push_back(i);
									intersection_points_1.push_back(*p);
								}
							}
						no_ls_interst_vec1.push_back(false);
					}

					else {
						no_ls_interst_vec1.push_back(true); // No intersection between line-segment
						//std::cout << "The segment and the plane-plane intersection do not intersect." << std::endl;
					}
				}
				if (VERBOSE == 1){
					std::cout << "###########################" << std::endl;
				}

				if (point_interst) {

					// Remove duplicate points

					list_seg_index_unique<Point_3_, NT_MP>(intersection_points_1, list_seg_interst1);
					if (VERBOSE == 1){
						std::cout << "Size of intersection_points_1 : " << intersection_points_1.size() <<std::endl;
					}

					int it_(0);

					for (std::list<Point_3_>::iterator it=intersection_points_1.begin(); it!=intersection_points_1.end(); ++it){

						intersection_points_1_out.push_back(CGAL::NT_MP_to_NT<Point_3_, Kernel::Point_3>(*it));

						std::list<int>::iterator it_seg = list_seg_interst1.begin();
						std::advance(it_seg, it_);

						//std::cout << ' ' << *it << std::endl;
						if (VERBOSE == 1){
							std::cout << "The segment and the plane-plane intersection intersect on a point: "<<
									*it << " on the segment " << *it_seg << std::endl;
						}
						num_intersec_1 ++;
						it_++;
					}
					if (VERBOSE == 1){
						std::cout << "Number of intersection between the polygon 1 and the intersection line = " << num_intersec_1 << std::endl;
					}
				}
			}


			//##########################
			// Ellipse 2
			if (VERBOSE == 1){
				std::cout << "###########################" << std::endl;
			}

			{
				unsigned int i = 0, end = polygon2.number_of_vertices();
				for( ; i < end; ++i) {
					vertex_descriptor vb_seg2(i);
					vertex_descriptor ve_seg2(i+1);
					vertex_descriptor ve_seg2_0(0);

					if (i==end-1)
						ve_seg2 = ve_seg2_0;

					//std::cout << "Check intersection in a line " << Line_Int << std::endl;

					Segment_3_ mesh_2_seg = Segment_3_(CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>( polygon2.point(vb_seg2) ), CGAL::NT_to_NT_MP<Kernel::Point_3, Point_3_>(  polygon2.point(ve_seg2)) );
					CGAL::cpp11::result_of<Kernel_MP::Intersect_3(Segment_3_, Line_3_)>::type result_sl2 = CGAL::intersection(mesh_2_seg, Line_Int);

					if (result_sl2) {
						if (const Segment_3_* s = boost::get<Segment_3_>(&*result_sl2)) {
							if (VERBOSE == 1){
								std::cout << "The segment lies on the plane-plane intersection." << std::endl;
								//std::cout << *s << std::endl;
							}
						}
						else
							if (const Point_3_* p = boost::get<Point_3_ >(&*result_sl2)) {
								//std::cout << "The segment and the plane-plane intersection intersect on a point." << std::endl;
								//std::cout << *p << std::endl;

								if (*p == polygon2.point(ve_seg2)){
									if (VERBOSE == 1){
										std::cout << "The segment and the plane-plane intersection intersect on the vertex linking the segment " << i
												<< " and the segment "<< i+1 << std::endl;
										std::cout << "Choose the segment " << i+1  << " as the segment of intersection."<< std::endl;
									}
								}
								else{
									if (VERBOSE == 1){
										std::cout << "The segment and the plane-plane intersection intersect on a point: "<<
												*p << " on the segment " << i << std::endl;
									}
									point_interst2 =true;
									list_seg_interst2.push_back(i);
									intersection_points_2.push_back(*p);
								}
							}
						no_ls_interst_vec2.push_back(false);
					}

					else {
						no_ls_interst_vec2.push_back(true); // No intersection between line-segment
						//std::cout << "The segment and the plane-plane intersection do not intersect." << std::endl;
					}
				}
				if (VERBOSE == 1){
					std::cout << "###########################" << std::endl;
				}

				if (point_interst2) {

					// Remove duplicate points

					list_seg_index_unique<Point_3_, NT_MP>(intersection_points_2, list_seg_interst2);

					if (VERBOSE == 1){
						std::cout << "Size of intersection_points_2 : " << intersection_points_2.size() <<std::endl;
					}

					int it_(0);

					for (std::list<Point_3_>::iterator it=intersection_points_2.begin(); it!=intersection_points_2.end(); ++it){

						intersection_points_2_out.push_back(CGAL::NT_MP_to_NT<Point_3_, Kernel::Point_3>(*it));

						std::list<int>::iterator it_seg = list_seg_interst2.begin();
						std::advance(it_seg, it_);

						//std::cout << ' ' << *it << std::endl;
						if (VERBOSE == 1){
							std::cout << "The segment and the plane-plane intersection intersect on a point: "<<
									*it << " on the segment " << *it_seg << std::endl;
						}
						num_intersec_2 ++;
						it_++;
					}

					if (VERBOSE == 1){
						std::cout << "Number of intersection between the polygon 2 and the intersection line = " << num_intersec_2 << std::endl;
					}
				}
			}
		}
		else {
			const Plane_3_* pl = boost::get<Plane_3_ >(&*result);
			//std::cout << *p << std::endl;
			std::cout << "Two planes are coincident" << std::endl;}

	}

	output_.num_intersec_1_ = num_intersec_1;
	output_.num_intersec_2_ = num_intersec_2;
	output_.intersection_points_1_ = intersection_points_1_out;
	output_.intersection_points_2_ = intersection_points_2_out;
	output_.list_seg_interst1_ = list_seg_interst1;
	output_.list_seg_interst2_ = list_seg_interst2;

	return output_;
}

} // end namespace CGAL

#endif /* PLANE_PLANE_INTERSECTION_CONVERT2MP_HH_ */
