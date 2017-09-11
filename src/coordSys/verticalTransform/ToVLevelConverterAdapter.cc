#include "fimex/coordSys/verticalTransform/ToVLevelConverterAdapter.h"

#include "fimex/CDM.h"
#include "fimex/CDMReader.h"
#include "fimex/coordSys/CoordinateSystem.h"
#include "fimex/coordSys/verticalTransform/VerticalConverter.h"
#include "fimex/coordSys/verticalTransform/VerticalTransformationUtils.h"
#include "fimex/Data.h"
#include "fimex/Logger.h"
#include "fimex/SliceBuilder.h"

namespace MetNoFimex {

using std::vector;

static LoggerPtr logger = getLogger("fimex.ToVLevelConverterAdapter");

static const size_t NOTSET = ~0ul;

ToVLevelConverterAdapter::ToVLevelConverterAdapter(CDMReader_p reader, boost::shared_ptr<const CoordinateSystem> cs,
                                                   VerticalConverterPtr converter, size_t unLimDimPos)
    : reader_(reader)
    , converter_(converter)
    , unlimitedTimePos_(NOTSET)
{
    const std::vector<std::string>& shape = converter_->getShape();
    const std::set<std::string> shapedims(shape.begin(), shape.end());

    if (CoordinateSystem::ConstAxisPtr xax = cs->getGeoXAxis()) {
        const std::string& xdim0 = xax->getShape().front();
        if (shapedims.count(xdim0))
            varGeoX_ = xdim0;
    }

    if (CoordinateSystem::ConstAxisPtr yax = cs->getGeoYAxis()) {
        const std::string& ydim0 = yax->getShape().front();
        if (shapedims.count(ydim0))
            varGeoY_ = ydim0;
    }

    if (CoordinateSystem::ConstAxisPtr tax = cs->getTimeAxis()) {
        const std::string& tdim0 = tax->getShape().front();
        if (shapedims.count(tdim0)) {
            varTime_ = tdim0;

            const CDMDimension& tDim = reader_->getCDM().getDimension(tdim0);
            if (tDim.isUnlimited())
                unlimitedTimePos_ = unLimDimPos;
        }
    }
}

vector<double> ToVLevelConverterAdapter::operator()(size_t x, size_t y, size_t t)
{
    const SliceBuilder sb = prepareSliceBuilder(x, y, t);

    DataPtr data = converter_->getDataSlice(sb);
    boost::shared_array<double> array = data->asDouble();
    return vector<double>(&array[0], &array[0] + data->size());
}

bool ToVLevelConverterAdapter::isValid(double val, size_t x, size_t y, size_t t)
{
    const SliceBuilder sb = prepareSliceBuilder(x, y, t);

    DataPtr data = converter_->getValiditySlice(sb, std::vector<double>(1, val));
    return data->getLongLong(0) != 0;
}

SliceBuilder ToVLevelConverterAdapter::prepareSliceBuilder(size_t x, size_t y, size_t t) const
{
    LOG4FIMEX(logger, Logger::DEBUG, "prepareSliceBuilder: about to create slicebuilder");
    SliceBuilder sb = createSliceBuilder(reader_->getCDM(), converter_);

    if (!varGeoX_.empty())
        sb.setStartAndSize(varGeoX_, x, 1);

    if (!varGeoY_.empty())
        sb.setStartAndSize(varGeoY_, y, 1);

    if (!varTime_.empty()) {
        if (unlimitedTimePos_ != NOTSET)
            t = unlimitedTimePos_;
        sb.setStartAndSize(varTime_, t, 1);
    }
    return sb;
}

} // namespace MetNoFimex
