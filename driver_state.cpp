#include "driver_state.h"
#include <cstring>
#include <limits>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color=0;
    state.image_depth=0;
    //std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;


 	state.image_color = new pixel[width*height];
	state.image_depth = new float[width*height];	//adds depth
	
	for(int i = 0; i < width * height; i++)
	{
       	state.image_color[i] = make_pixel(0, 0, 0);

	state.image_depth[i] = std::numeric_limits<float>::max();	
    	}
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{

	const data_geometry *vertexe[3];
    	data_geometry outgoing[3];
    	data_vertex incoming[3];

    	switch (type)		//needs other switch types
	{

       	case render_type::triangle: 
	{
        	int k = 0;
          	int numtri = state.num_vertices / 3;
          	
		for (int i = 0; i < numtri; i++)
		{
             		for (int j = 0; j < 3; j++, k += state.floats_per_vertex)
			{
                		incoming[j].data = &state.vertex_data[k];
                		outgoing[j].data = incoming[j].data;
                		state.vertex_shader(incoming[j], outgoing[j], state.uniform_data);
                		vertexe[j] = &outgoing[j];
             		}
             	//rasterize_triangle(state,*vertexe[0],*vertexe[1],*vertexe[2]);
		clip_triangle(state, *vertexe[0], *vertexe[1], *vertexe[2], 0);			//for when we use clip
         	}
          	break;
       	}                

	case render_type::indexed: 
	{
        	int numtri = state.num_triangles*3;
          	for(int i = 0; i<numtri; i += 3)
		{
             		for (int j = 0; j< 3; ++j)
			{
                		incoming[j].data = &state.vertex_data[state.index_data[i+j]*state.floats_per_vertex];
                		outgoing[j].data = incoming[j].data;
                		state.vertex_shader(incoming[j], outgoing[j], state.uniform_data);
                		vertexe[j] = &outgoing[j];
             		}
             	clip_triangle(state, *vertexe[0], *vertexe[1], *vertexe[2], 0);
          	}
   	break;
       	}

       	case render_type::fan: 
	{
          	int numtri = state.num_vertices;
          	for (int i = 0; i<numtri; i++)
		{
             		for (int j = 0; j< 3; ++j)
			{
                		if (j == 0)
				{
                   		incoming[j].data = state.vertex_data + ((0+j)*state.floats_per_vertex);
                		}
                		else
				{
                   		incoming[j].data = state.vertex_data + ((i+j)*state.floats_per_vertex);
                		}
                	
			outgoing[j].data = incoming[j].data;
                	state.vertex_shader(incoming[j], outgoing[j], state.uniform_data);
                	vertexe[j] = &outgoing[j];
             		}
             	clip_triangle(state, *vertexe[0], *vertexe[1], *vertexe[2], 0);
          	}
	break;
       	}

       	case render_type::strip: 
	{
          	int numtri = state.num_vertices - 2;
          	for (int i = 0; i < numtri; ++i)
		{
             		for (int j = 0; j < 3; ++j)
			{
                		incoming[j].data = &state.vertex_data[(i + j) * state.floats_per_vertex];
                		outgoing[j].data = incoming[j].data;
                		state.vertex_shader(incoming[j], outgoing[j], state.uniform_data);
                		vertexe[j] = &outgoing[j];
             		}
             		
			clip_triangle(state, *vertexe[0], *vertexe[1], *vertexe[2], 0);
          	}
    	break;
       	}
		

	default:
	{
	break;
	}

//    std::cout<<"TODO: implement rendering."<<std::endl;
}
}



	// Helper funciton calculates area of triangle
	
	float compute_triangle_area(int x0, int x1, int x2, int y0, int y1, int y2) 
	{
     	return 0.5 * (((x1 * y2) - (x2 * y1)) - ((x0 * y2) - (x2 * y0)) + ((x0 * y1) - (x1 * y0)));
     	}




// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2,int face)
    {
    if(face==6)
    {
        rasterize_triangle(state, v0, v1, v2);
        return;
    }	else
	{
//	const data_geometry *vertexe[3];

  	
	}
	
//    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
   clip_triangle(state, v0, v1, v2,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2)
{


	float x[3];
    	float y[3];
	float z[3];	

 	x[0] = 0.5 * ((state.image_width*v0.gl_Position[0]/v0.gl_Position[3])+state.image_width-1.0);
   	x[1] = 0.5 * ((state.image_width*v1.gl_Position[0]/v1.gl_Position[3])+state.image_width-1.0);
    	x[2] = 0.5 * ((state.image_width*v2.gl_Position[0]/v2.gl_Position[3])+state.image_width-1.0);

    	y[0] = 0.5 * ((state.image_height*v0.gl_Position[1]/v0.gl_Position[3])+state.image_height-1.0);
    	y[1] = 0.5 * ((state.image_height*v1.gl_Position[1]/v1.gl_Position[3])+state.image_height-1.0);
    	y[2] = 0.5 * ((state.image_height*v2.gl_Position[1]/v2.gl_Position[3])+state.image_height-1.0);

	z[0] = 0.5 * ((state.image_width*v0.gl_Position[2]/v0.gl_Position[3])+state.image_width-1.0);
   	z[1] = 0.5 * ((state.image_width*v1.gl_Position[2]/v1.gl_Position[3])+state.image_width-1.0);
   	z[2] = 0.5 * ((state.image_width*v2.gl_Position[2]/v2.gl_Position[3])+state.image_width-1.0);	


 	float lowest_x = std::min(std::min(x[0],x[1]),x[2]);
	if(lowest_x < 0)
	{
	lowest_x = 0;
	}    	
	float lowest_y = std::min(std::min(y[0],y[1]),y[2]);
	if(lowest_y < 0)
	{
	lowest_y = 0;	
	}	

   	float highest_x = std::max(std::max(x[0],x[1]),x[2]);
	if(highest_x > state.image_width)
	{
	highest_x = state.image_width - 1;
	}
	float highest_y = std::max(std::max(y[0],y[1]),y[2]);
	if(highest_y > state.image_height){
	highest_y = state.image_height - 1;
	}

 	float tri_area = compute_triangle_area(x[0],x[1],x[2],y[0],y[1],y[2]);

 	float* temp_data = new float[MAX_FLOATS_PER_VERTEX];
    	data_fragment fragment_data{temp_data};
    	data_output fragment_out;

	float alpha, beta, gamma;
	float alphacon, betacon, gammacon;

    	for (int cur_x = lowest_x; cur_x <= highest_x; cur_x++) //fills triangle
	{
       		for (int cur_y = lowest_y; cur_y <= highest_y; cur_y++)		//this part needs to be changed to add color
		{
          		alpha = compute_triangle_area(cur_x, x[1], x[2], cur_y, y[1], y[2]) / tri_area;
          		beta = compute_triangle_area(x[0], cur_x, x[2], y[0], cur_y, y[2]) / tri_area;
          		gamma = compute_triangle_area(x[0], x[1], cur_x, y[0],  y[1], cur_y) / tri_area;

			if (alpha >= 0 && beta >= 0 && gamma >= 0){	//fills only half of the square
				
				float z_val = alpha*z[0]+beta*z[1]+gamma*z[2];

				alphacon = alpha;
                        	betacon = beta;
                        	gammacon = gamma;

				if(z_val<state.image_depth[cur_x+(cur_y*state.image_width)])
				{
  					state.image_depth[cur_x + (cur_y*state.image_width)] = z_val;			
	
					for (int k=0; k<state.floats_per_vertex; k++)
					{
						float c;
						
						switch (state.interp_rules[k])
						{
							case interp_type::flat:
								fragment_data.data[k] = v0.data[k];
							break;

							case interp_type::smooth:
                          					c = (alphacon/v0.gl_Position[3]) + (betacon/v1.gl_Position[3]) + (gammacon/v2.gl_Position[3]);

                          					alpha = alphacon/(v0.gl_Position[3]*c);
                          					beta = betacon/(v1.gl_Position[3]*c);
                          					gamma = gammacon/(v2.gl_Position[3]*c);

                          					fragment_data.data[k] =alpha*v0.data[k]+beta*v1.data[k]+gamma*v2.data[k];
                          				break;

                       					case interp_type::noperspective:
                          					fragment_data.data[k] = alphacon*v0.data[k]+betacon*v1.data[k]+gammacon*v2.data[k];
                          				break;

                       					default:
                          				break;
						}									
					}
					state.image_color[cur_x + (cur_y * state.image_width)] = make_pixel(fragment_out.output_color[0]*255,fragment_out.output_color[1]*255,fragment_out.output_color[2]*255); //sets color
					//state.image_color[cur_x + (cur_y * state.image_width)] = make_pixel(255,255,255); //sets pixel to white
					state.fragment_shader(fragment_data, fragment_out, state.uniform_data);
				}
			}
				
		}}
}
