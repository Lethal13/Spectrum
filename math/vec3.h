#ifndef MATH_VEC3_H
#define MATH_VEC3_H

typedef struct
{
	float x;
	float y;
	float z;
} vec3_t;

// Vector 3D functions declarations.
float vec3_t_length(vec3_t v);
vec3_t vec3_t_add(vec3_t a, vec3_t b);
vec3_t vec3_t_sub(vec3_t a, vec3_t b);
vec3_t vec3_t_mul(vec3_t v, float factor);
vec3_t vec3_t_div(vec3_t v, float factor);
vec3_t vec3_t_cross(vec3_t a, vec3_t b);
float vec3_t_dot(vec3_t a, vec3_t b);
void vec3_t_normalize(vec3_t* v);

vec3_t vec3_t_rotate_x(vec3_t v, float angle);
vec3_t vec3_t_rotate_y(vec3_t v, float angle);
vec3_t vec3_t_rotate_z(vec3_t v, float angle);

// Vector 3D functions definitions.
float vec3_t_length(vec3_t v)
{
    return (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_t_add(vec3_t a, vec3_t b)
{
    vec3_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return result;
}

vec3_t vec3_t_sub(vec3_t a, vec3_t b)
{
    vec3_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return result;
}

vec3_t vec3_t_mul(vec3_t v, float factor)
{
    vec3_t result = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor
    };
    return result;
}

vec3_t vec3_t_div(vec3_t v, float factor)
{
    vec3_t result = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor
    };
    return result;
}

vec3_t vec3_t_cross(vec3_t a, vec3_t b)
{
    vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}

float vec3_t_dot(vec3_t a, vec3_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_t_normalize(vec3_t* v)
{
    float length = (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

vec3_t vec3_t_rotate_x(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * (float)cos(angle) - v.z * (float)sin(angle),
        .z = v.y * (float)sin(angle) + v.z * (float)cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_t_rotate_y(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x * (float)cos(angle) - v.z * (float)sin(angle),
        .y = v.y,
        .z = v.x * (float)sin(angle) + v.z * (float)cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_t_rotate_z(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x * (float)cos(angle) - v.y * (float)sin(angle),
        .y = v.x * (float)sin(angle) + v.y * (float)cos(angle),
        .z = v.z
    };
    return rotated_vector;
}


#endif
