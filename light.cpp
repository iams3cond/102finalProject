
#include "ray.h"

static pparm_t light_parse[] =
{
   {"location",   3, 8, "%lf", 0},
   {"emissivity",  3, 8, "%lf", 0}
};

#define NUM_ATTRS (sizeof(light_parse) / sizeof(pparm_t))

/* Create a new light description */
light_t::light_t(FILE *in, model_t *model, int attrmax)
{
// - parse the required attribute values
// - put the light_t class instance into the light list
   int mask;
   //list_t *list;
   char attrname[NAME_LEN];
   cookie = LGT_COOKIE;
   fscanf(in, "%s", name);
   

   attrname[0] = 0;
   fscanf(in, "%s", attrname);
   assert(attrname[0] == '{');

   light_parse[0].loc = &location;
   light_parse[1].loc = &emissivity;

   mask = parser(in, light_parse, NUM_ATTRS, attrmax);
   assert(mask == 3);

   model->lgts->add((void *)this);
   //list->add((void *)this);

}

//The light list printer.

void light_t::printer(FILE  *out) 
{
   assert(cookie == LGT_COOKIE);
   fprintf(out, "%-12s %s \n", "light", name);
   fprintf(out, "%-12s %5.1lf %5.1lf %5.1lf \n",
                 "location", location.x, location.y,
                 location.z);
   fprintf(out, "%-12s %5.1lf %5.1lf %5.1lf \n",
                 "emissivity", emissivity.r, emissivity.g,
                 emissivity.b);
}

/* Produce a formatted print of the light list */
void light_list_print(model_t *model, FILE *out)
{
 light_t *light;
 list_t *list = model->lgts;
 list->reset();
 
   while (list->not_end())
   {
      light = (light_t *)list->get_entity();
      light->printer(out);
      fprintf(out, "\n");
      list->next_link();
   }

}





void light_t::illuminate(
model_t     *model, 
vec_t       *base,
object_t    *hitobj, /* The object hit by the ray    */
drgb_t      *pixel)   /* add illumination here       */
{
   vec_t    dir;    // unit direction to light from hitpt
   object_t *obj;   // closest object in dir to light
   double   close;  // dist to closest object in dir to light
   double   cos;    // of angle between normal and dir to light
   double   dist;   // to the light from hitpoint
   vec_t    last_hitpt; // from hitobj argument
   vec_t    last_normal;
   drgb_t   diffuse = {0.0, 0.0, 0.0};
/* Compute the distance from the hit to the light and a unit */
/* vector in the direction of the light from hitpt           */
        hitobj->getlast_hitpt(&last_hitpt);
        hitobj->getlast_normal(&last_normal);
        
        vec_diff(&last_hitpt, &location, &dir);
        dist = sqrt(vec_dot(&dir, &dir));
        vec_unit(&dir, &dir);

/* Test the object for self-occlusion and return if occluded */

        cos = vec_dot(&last_normal, &dir);
        if(cos < 0 )
        {
                return;
        }

/* Ask find_closest_object() if a ray fired toward the light */
/* hits a "regular" object. Pass "hitobj" as the "lasthit"   */
/* parameter so it won't be hit again at distance 0.         */

        obj = find_closest_object(model, &last_hitpt, 
                        &dir, hitobj, &close);

/* If an object is hit and the distance to the hit is   */
/* closer to the hitpoint than the light is, return     */

        if(obj != NULL && close < dist)
        {
                return;
        }

/* Arriving at this point means the light does illuminate */
/* object. Ask hitobj->getdiff() to return diffuse        */
/* reflectivity                                           */

        hitobj->getdiffuse(&diffuse);

/* Multiply componentwise the diffuse reflectivity by     */
/* the emissivity of the light.                           */

        pix_prod(&emissivity, &diffuse, &diffuse);

/* Scale the resulting diffuse reflectivy by cos/dist     */

        pix_scale(cos/dist, &diffuse, &diffuse);

/* Add scaled value to *pixel.                            */

        pix_sum(pixel, &diffuse, pixel);
}



