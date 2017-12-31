#ifndef BATCHMANAGER_H
#define BATCHMANAGER_H

#include "Bang/GL.h"
#include "Bang/Array.h"
#include "Bang/Batch.h"

NAMESPACE_BANG_BEGIN

class BatchManager
{
public:
	BatchManager();
	virtual ~BatchManager();

    void AddGeometry(const Array<Vector3> &positions,
                     const Array<Vector3> &normals,
                     const Array<Vector2> &uvs,
                     const BatchParameters &batchParams,
                     bool incrementalZ);

    void Render();

    void ClearBatches();

private:
    double m_incrementalZ = 0.0;
    Array<Batch> m_currentBatches;
};

NAMESPACE_BANG_END

#endif // BATCHMANAGER_H

