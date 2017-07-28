#ifndef STATICLIB_MACROS
#define STATICLIB_MACROS

/* Macros used to enforce the inclusion of object files (and static initialization) into 
 * a static library. G++ (or more accurately ld) ignores object files that are not explicitely
 * used. This macro hack circumvents this by 'using' the object files through Env::pull() */

#define DECLARE_OBJECT(object_name) void object_name##_to_circumvent_static_linking_problem() {}
#define CALL_OBJECT(object_name)    extern void object_name##_to_circumvent_static_linking_problem(); object_name##_to_circumvent_static_linking_problem()

#endif
