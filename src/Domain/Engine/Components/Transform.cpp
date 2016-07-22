#include "Transform.h"
#include "FileReader.h"
#include "GameObject.h"

Transform::Transform() : m_localPosition(Vector3(0.0f))
                        ,m_localRotation(Quaternion())
                        ,m_localScale(Vector3(1.0f))
                        #ifdef BANG_EDITOR
                        ,m_localEuler(Vector3(0.0f))
                        #endif
{
    #ifdef BANG_EDITOR
    m_inspectorComponentInfo.AddSlotInfos(
    {
        new InspectorVFloatSWInfo(
            "Position", {m_localPosition.x, m_localPosition.y, m_localPosition.z}
        ),
        new InspectorVFloatSWInfo(
            "Rotation", {m_localEuler.x, m_localEuler.y, m_localEuler.z}
        ),
        new InspectorVFloatSWInfo(
            "Scale", {m_localScale.x, m_localScale.y, m_localScale.z}
        )
    });
    #endif
}

void Transform::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Transform *t = static_cast<Transform*>(clone);
    t->SetLocalPosition(GetLocalPosition());
    t->SetLocalRotation(GetLocalRotation());
    t->SetLocalScale(GetLocalScale());
}


ICloneable *Transform::Clone() const
{
    Transform *t = new Transform();
    CloneInto(t);
    return t;
}

Transform::~Transform()
{
}

void Transform::SetLocalPosition(const Vector3 &p)
{
    m_localPosition = p;
}
void Transform::SetPosition(const Vector3 &p)
{
    if (!gameObject->parent) SetLocalPosition(p);
    else SetLocalPosition(-gameObject->parent->transform->GetPosition() + p);
}
void Transform::TranslateLocal(const Vector3 &translation)
{
    SetLocalPosition(GetLocalPosition() + translation);
}
void Transform::Translate(const Vector3 &translation)
{
    SetPosition(GetPosition() + translation);
}



void Transform::SetLocalRotation(const Quaternion &q)
{
    m_localRotation = q.Normalized();
    m_localEuler = Quaternion::EulerAngles(m_localRotation).ToDegrees();
}
void Transform::SetLocalEuler(const Vector3 &degreesEuler)
{
    m_localEuler = degreesEuler;
    Vector3 rads = degreesEuler.ToRadians();
    Quaternion qx = Quaternion::AngleAxis(rads.x, Vector3::right);
    Quaternion qy = Quaternion::AngleAxis(rads.y, Vector3::up);
    Quaternion qz = Quaternion::AngleAxis(rads.z, Vector3::forward);
    m_localRotation = (qz * qy * qx).Normalized();
}
void Transform::SetLocalEuler(float x, float y, float z)
{
    SetLocalEuler(Vector3(x,y,z));
}

void Transform::SetRotation(const Quaternion &q)
{
    if (!gameObject->parent) SetLocalRotation(q.Normalized());
    else SetLocalRotation(Quaternion(-gameObject->parent->transform->GetRotation() * q.Normalized()));
}
void Transform::SetEuler(const Vector3 &degreesEuler)
{
    if (!gameObject->parent) SetLocalEuler(degreesEuler);
    else SetLocalEuler(-gameObject->parent->transform->GetEuler() + degreesEuler);
}
void Transform::SetEuler(float x, float y, float z)
{
    SetEuler(Vector3(x,y,z));
}

void Transform::RotateLocal(const Quaternion &r)
{
    SetLocalRotation(Quaternion(GetLocalRotation() * r.Normalized()));
}
void Transform::RotateLocalEuler(const Vector3 &degreesEuler)
{
    SetLocalEuler(GetLocalEuler() + degreesEuler);
}
void Transform::Rotate(const Quaternion &r)
{
    SetRotation(Quaternion(r.Normalized() * GetRotation()));
}
void Transform::RotateEuler(const Vector3 &degreesEuler)
{
    SetEuler(GetEuler() + degreesEuler);
}

#ifdef BANG_EDITOR
void Transform::SetLocalRotationFromInspector(const Quaternion &q)
{
    m_localRotation = q.Normalized();
}
#endif




void Transform::SetScale(float s)
{
    SetScale(Vector3(s));
}

void Transform::SetScale(const Vector3 &v)
{
    SetLocalScale(1.0f / gameObject->parent->transform->GetScale() * v);
}

void Transform::SetLocalScale(float s)
{
    SetLocalScale(Vector3(s));
}

void Transform::SetLocalScale(const Vector3 &s)
{
    m_localScale = s;
}




Vector3 Transform::TransformPoint(const Vector3 &point) const
{
    Matrix4 m;
    if (!gameObject->parent) return point;
    gameObject->parent->transform->GetModelMatrix(m);
    return Vector3((m * glm::vec4(point, 1)).xyz());
}
Vector3 Transform::InverseTransformPoint(const Vector3 &point) const
{
    Matrix4 m;
    if (!gameObject->parent) return point;
    gameObject->parent->transform->GetModelMatrix(m);
    return Vector3((m.Inversed() * glm::vec4(point, 1)).xyz());
}
Vector3 Transform::TransformDirection(const Vector3 &dir) const
{
    Matrix4 m;
    if (!gameObject->parent) return dir;
    gameObject->parent->transform->GetModelMatrix(m);
    return Vector3((m * glm::vec4(dir, 0)).xyz());
}
Vector3 Transform::InverseTransformDirection(const Vector3 &dir) const
{
    Matrix4 m;
    if (!gameObject->parent) return dir;
    gameObject->parent->transform->GetModelMatrix(m);
    return Vector3((m.Inversed() * glm::vec4(dir, 0)).xyz());
}




Vector3 Transform::LocalToWorldPoint(const Vector3 &point) const
{
    return TransformPoint(point);
}
Vector3 Transform::WorldToLocalPoint(const Vector3 &point) const
{
    return InverseTransformPoint(point);
}
Vector3 Transform::LocalToWorldDirection(const Vector3 &dir) const
{
    return TransformDirection(dir);
}
Vector3 Transform::WorldToLocalDirection(const Vector3 &dir) const
{
    return InverseTransformDirection(dir);
}


Vector3 Transform::LocalToObjectPoint(const Vector3 &point) const
{
    Matrix4 m;
    GetObjectModelMatrix(m);
    return Vector3((m.Inversed() * glm::vec4(point, 1)).xyz());
}
Vector3 Transform::LocalToObjectDirection(const Vector3 &dir) const
{
    Matrix4 m;
    GetObjectModelMatrix(m);
    return Vector3((m.Inversed() * glm::vec4(dir, 0)).xyz());
}
Vector3 Transform::ObjectToLocalPoint(const Vector3 &point) const
{
    Matrix4 m;
    GetObjectModelMatrix(m);
    return Vector3((m * glm::vec4(point, 1)).xyz());
}
Vector3 Transform::ObjectToLocalDirection(const Vector3 &dir) const
{
    Matrix4 m;
    GetObjectModelMatrix(m);
    return Vector3((m * glm::vec4(dir, 0)).xyz());
}


Vector3 Transform::ObjectToWorldPoint(const Vector3 &point) const
{
    return LocalToWorldPoint(ObjectToLocalPoint(point));
}
Vector3 Transform::ObjectToWorldDirection(const Vector3 &dir) const
{
    return LocalToWorldDirection(ObjectToLocalDirection(dir));
}
Vector3 Transform::WorldToObjectPoint(const Vector3 &point) const
{
    return LocalToObjectPoint(WorldToLocalPoint(point));
}
Vector3 Transform::WorldToObjectDirection(const Vector3 &dir) const
{
    return LocalToObjectDirection(WorldToLocalDirection(dir));
}




void Transform::GetObjectModelMatrix(Matrix4 &m) const
{
    Matrix4 T = Matrix4::TranslateMatrix(GetLocalPosition());
    Matrix4 R = Matrix4::RotateMatrix(GetLocalRotation());
    Matrix4 S = Matrix4::ScaleMatrix(GetLocalScale());

    m = T * R * S;
}

void Transform::GetModelMatrix(Matrix4 &m) const
{
    GetObjectModelMatrix(m);
    if (gameObject->parent)
    {
        Matrix4 mp;
        gameObject->parent->transform->GetModelMatrix(mp);
        m = mp * m;
    }
}

void Transform::GetNormalMatrix(Matrix4 &m) const
{
    GetModelMatrix(m);
    m = m.Inversed().Transposed();
}


void Transform::LookAt(const Vector3 &target, const Vector3 &_up)
{
    Assert(target != m_localPosition, "LookAt target is the same as position.", return);
    Vector3 up = _up.Normalized();
    SetRotation(Quaternion::LookDirection(target - GetPosition(), up) );
}

void Transform::LookAtLocal(const Vector3 &target, const Vector3 &_up)
{
    Assert(target != m_localPosition, "LookAt target is the same as position.", return);
    Vector3 up = _up.Normalized();
    SetLocalRotation(Quaternion::LookDirection(target - GetLocalPosition(), up) );
}

void Transform::LookInDirection(const Vector3 &dir, const Vector3 &up)
{
    LookAt(GetPosition() + dir * 99, up);
}

void Transform::LookInLocalDirection(const Vector3 &dir, const Vector3 &up)
{
    LookAtLocal(GetLocalPosition() + dir * 99, up);
}

Vector3 Transform::GetLocalPosition() const
{
    return m_localPosition;
}

Vector3 Transform::GetPosition() const
{
    if (!gameObject->parent) return GetLocalPosition();
    return gameObject->parent->transform->GetPosition() + GetLocalPosition();
}

Quaternion Transform::GetLocalRotation() const
{
    return m_localRotation;
}

Quaternion Transform::GetRotation() const
{
    if (!gameObject->parent) return GetLocalRotation();
    return gameObject->parent->transform->GetRotation() * GetLocalRotation();
}

Vector3 Transform::GetLocalEuler() const
{
    return m_localEuler;
}

Vector3 Transform::GetEuler() const
{
    if (!gameObject->parent) return GetLocalEuler();
    return gameObject->parent->transform->GetEuler() + GetLocalEuler();
}

Vector3 Transform::GetLocalScale() const
{
    return m_localScale;
}

Vector3 Transform::GetScale() const
{
    if (!gameObject->parent) return GetLocalScale();
    return gameObject->parent->transform->GetScale() * GetLocalScale();
}

Vector3 Transform::GetForward() const
{
    return  ObjectToWorldDirection(Vector3::forward).Normalized();
}

Vector3 Transform::GetBack() const
{
    return -GetForward();
}

Vector3 Transform::GetRight() const
{
    return  ObjectToWorldDirection(Vector3::right).Normalized();
}

Vector3 Transform::GetLeft() const
{
    return -GetRight();
}

Vector3 Transform::GetUp() const
{
    return ObjectToWorldDirection(Vector3::up).Normalized();
}

Vector3 Transform::GetDown() const
{
    return -GetUp();
}


const std::string Transform::ToString() const
{
    using std::operator<<;

    std::ostringstream msg;

    Vector3 euler = Quaternion::EulerAngles(m_localRotation);
    msg << "[" <<
           "Transform: " << std::endl <<
           "   Position: (" << m_localPosition.x << ", " << m_localPosition.y << ", " << m_localPosition.z << ")"  << std::endl <<
           "   Rotation: (" << glm::degrees(euler.x) << ", " << glm::degrees(euler.y) << ", " << glm::degrees(euler.z) << ")" << std::endl <<
           "   Scale: (" << m_localScale.x << ", " << m_localScale.y << ", " << m_localScale.z << ")" << std::endl <<
           "]" << std::endl;

    return msg.str();
}
#ifdef BANG_EDITOR

InspectorWidgetInfo* Transform::GetComponentInfo()
{
    Vector3 pos = GetPosition();
    Vector3 rotEuler = GetEuler();
    Vector3 scale = GetScale();

    static_cast<InspectorVFloatSWInfo*>(m_inspectorComponentInfo.GetSlotInfo(0))->m_value =
        {pos.x, pos.y, pos.z};

    static_cast<InspectorVFloatSWInfo*>(m_inspectorComponentInfo.GetSlotInfo(1))->m_value =
        {rotEuler.x, rotEuler.y, rotEuler.z};

    static_cast<InspectorVFloatSWInfo*>(m_inspectorComponentInfo.GetSlotInfo(2))->m_value =
        {scale.x, scale.y, scale.z};

    return &m_inspectorComponentInfo;
}

void Transform::OnSlotValueChanged(InspectorWidget *source)
{
    std::vector<float> v;
    v = source->GetSWVectorFloatValue("Position");
    SetPosition(Vector3(v[0], v[1], v[2]));

    v = source->GetSWVectorFloatValue("Rotation");
    SetEuler(Vector3(v[0], v[1], v[2]));

    v = source->GetSWVectorFloatValue("Scale");
    SetScale(Vector3(v[0], v[1], v[2]));
}
#endif

void Transform::WriteInternal(std::ostream &f) const
{
    Component::WriteInternal(f);
    FileWriter::WriteVector3(GetLocalPosition(), f);
    FileWriter::WriteQuaternion(GetLocalRotation(), f);
    FileWriter::WriteVector3(GetLocalScale(), f);
}

void Transform::ReadInternal(std::istream &f)
{
    Component::ReadInternal(f);
    SetLocalPosition(FileReader::ReadVec3(f));
    SetLocalRotation(FileReader::ReadQuat(f));
    SetLocalScale(FileReader::ReadVec3(f));
}
