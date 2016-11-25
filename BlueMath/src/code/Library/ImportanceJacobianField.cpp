/*
  ImportanceJacobianField.cpp

  Li-Yi Wei
  10/29/2010

*/

#include <math.h>

#include "ImportanceJacobianField.hpp"
#include "SampledDistanceField.hpp"
#include "SequentialCounter.hpp"
#include "Utility.hpp"
#include "Exception.hpp"
#include "SobelFilter.hpp"

ImportanceJacobianField::ImportanceJacobianField(const Domain & domain, const Array<float> & importance, const float max_importance,  const float cell_size, const float r_value) : DiscreteJacobianField(domain, cell_size)
{
    Array<Matrix> inverse_jacobians;

    if(! Convert(domain, importance, max_importance, cell_size, r_value, inverse_jacobians)) throw Exception("ImportanceJacobianField::ImportanceJacobianField(): cannot convert importance into Jacobian");

    CommonConstruct(inverse_jacobians, r_value);
}

ImportanceJacobianField::~ImportanceJacobianField(void)
{
    // nothing else to do
}

vector<float> ImportanceJacobianField::DomainSize(const Array<float> & importance, const float cell_size)
{
    vector<float> domain_size(importance.Dimension());

    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = importance.Size(i)*cell_size;
    }

    return domain_size;
}

int ImportanceJacobianField::Convert(const Domain & domain, const Array<float> & importance_in, const float max_importance, const float cell_size, const float r_value, Array<Matrix> & inverse_jacobians)
{
    Array<float> importance(importance_in);

    // normalize
    {
        if(max_importance <= 0)
        {
            throw Exception("ImportanceJacobianField::Convert(): max_importance <= 0");
            return 0;
        }

        vector<float> values;
        if(!importance.Get(values)) throw Exception("ImportanceJacobianField::Convert(): cannot get values");

        float max_value = 0;
        for(unsigned int i = 0; i < values.size(); i++)
        {
            if(max_value < values[i])
            {
                max_value = values[i];
            }
        }

        if(max_value > 0)
        {
            const float ratio = max_importance/max_value;

            for(unsigned int i = 0; i < values.size(); i++)
            {
                values[i] *= ratio;
            }
        }

        if(!importance.Put(values)) throw Exception("ImportanceJacobianField::Convert(): cannot put values");
    }

    vector<int> array_size;
    importance.Size(array_size);

    const int dimension = importance.Dimension();

    if(dimension != 2) throw Exception("ImportanceJacobianField::Convert(): support only 2D for now!");

    // resize the output if necessary
    int need_resize = (importance.Dimension() != inverse_jacobians.Dimension());
    
    for(int i = 0; i < importance.Dimension(); i++)
    {
        need_resize += (importance.Size(i) != inverse_jacobians.Size(i));
    }
    
    if(need_resize)
    {
        inverse_jacobians = Array<Matrix>(array_size);
    }

    // distance field
    SampledDistanceField distance_field(domain, importance, cell_size, r_value);

    // matrix element
    vector<int> element_size(2, dimension);
    Matrix element(element_size);
    if(! element.Identity()) 
    {
        return 0;
    }

    vector<int> element_index(2);

    // filter
    SobelFilter filter;

    // counter
    SequentialCounter counter(dimension, vector<int>(dimension, 0), Utility::Minus1(array_size));

    counter.Reset();
    vector<int> array_index;
    vector<float> gradient(dimension);
    float value;

    do
    {
        counter.Get(array_index);

        // compute gradient
        for(int orientation = 0; orientation < dimension; orientation++)
        {
            if(! filter.SetOrientation(orientation))
            {
                throw Exception("ImportanceJacobianField::Convert(): cannot set filter orientation");
            }

            gradient[orientation] = filter.Query(importance, array_index);
        }

        const float norm2 = (gradient[0]*gradient[0] + gradient[1]*gradient[1]);
        const float prod = gradient[0]*gradient[1];

        const float angle = (norm2 <= 0 ? 0 :  0.5*atan(-2.0*prod/(gradient[0]*gradient[0] - gradient[1]*gradient[1])) );

        const float sin2angle = sin(2*angle);

        const float part1 = 2 + norm2;
        const float part2 = 2/sin2angle*prod;

#if 0
        if((norm2 > 0) && (sin2angle == 0))
        {
            throw Exception("ImportanceJacobianField::Convert(): (norm2 > 0) && (sin2angle == 0)");
            return 0;
        }
#endif

        float a = 0;
        float b = 0;

        if(sin2angle == 0)
        {
            a = sqrt(1 + gradient[0]*gradient[0]);
            b = sqrt(1 + gradient[1]*gradient[1]);
        }
        else
        {
            if(part1 < fabs(part2))
            {
                throw Exception("ImportanceJacobianField::Convert(): part1 < fabs(part2)");
                return 0;
            }

            a = sqrt(0.5*(part1 - part2));
            b = sqrt(0.5*(part1 + part2));
        }

        const float cosangle = cos(angle);
        const float sinangle = sin(angle);

        // off-diagonal entries
        element.Clear((-a+b)*sinangle*cosangle);

        // diagonal entries
        if(! element.Put(vector<int>(2, 0), a*cosangle*cosangle + b*sinangle*sinangle))  throw Exception("ImportanceJacobianField::Convert(): cannot put element entry");
        if(! element.Put(vector<int>(2, 1), a*sinangle*sinangle + b*cosangle*cosangle))  throw Exception("ImportanceJacobianField::Convert(): cannot put element entry");

        // multiply by local_r
        const float local_r = distance_field.Query(array_index);

        if(local_r <= 0) throw Exception("ImportanceJacobianField::Convert(): local_r <= 0");

        if(! element.Multiply(1.0/local_r)) throw Exception("ImportanceJacobianField::Convert(): cannot multiply final element");

        if(! inverse_jacobians.Put(array_index, element)) throw Exception("ImportanceJacobianField::Convert(): cannot put array value");
    }
    while(counter.Next());

    // done
    return 1;
}
