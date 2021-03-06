/*   Greg Francis/Gfranci
     Cpsc102 Section 1
     10/20/15
     MPA#2
*/  


/*  raytrace.c  */

/*  find_closest_object() function
    arguments:
       model_t *model - the model structure
       vect_t *base - base of the ray (viewpoint)
       vect_t *dir - unit direction vector of the ray
       object_t *last_hit - the last hit object
       double *retdist - store distance to the hit point here

     	 if no object is hit, then retdist will have the value -1, 
	       and NULL will be returned 
		 if an object is hit, retdist will end up with the mindist
		    and minobj will be a non-NULL object that is returned 
*/


#include "ray.h"


object_t *find_closest_object(model_t  *model, vec_t  *base, vec_t  *dir,
                              object_t  *last_hit, double  *retdist)     {
   
   object_t  *minobj = NULL;
   object_t *temp;
   list_t *objlist = model->objs;
   double dist,mindist = 0;
   
   objlist->reset();
   while (objlist->not_end() == 1 )
   {
           temp = (object_t *)objlist->get_entity();
           
           if(temp == last_hit)
           {
                   objlist->next_link();
                   continue;
                 
           }
           dist = temp->hits(base, dir);
           if ((dist >= 0) && ((minobj == NULL)||(dist < mindist)))
           {
                
                   mindist = dist;
                   minobj = temp;
           }
           
           
           
           objlist->next_link();
           
        
         
   }
   
   *retdist = mindist;
   return(minobj);

}    

        
void add_illumination(model_t  *model, vec_t *base, object_t *hitobj,
                      drgb_t   *pixel)
{
        light_t *light; 
        list_t *list;
        
        list = model->lgts;
        list->reset();
        
        while (list->not_end())
        {
                light = (light_t *)list->get_entity();
                light->illuminate(model,base,hitobj,pixel);
                
                list->next_link();
        }

        return;



}

