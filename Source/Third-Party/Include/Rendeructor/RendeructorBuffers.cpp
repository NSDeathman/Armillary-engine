#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

void InstanceBuffer::Create(const void* data, int count, int stride) {
    m_count = count;
    m_stride = stride;
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateInstanceBuffer(data, count * stride, stride);
    }
}
