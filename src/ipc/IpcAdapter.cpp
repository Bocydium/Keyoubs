#include "IpcAdapter.hpp"

namespace keyoubs {

IpcAdapter::IpcAdapter(QObject *parent)
    : QObject(parent)
{
}

IpcAdapter::~IpcAdapter() = default;

} // namespace keyoubs
