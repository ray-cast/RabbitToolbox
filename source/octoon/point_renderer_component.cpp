#include <octoon/point_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(PointRendererComponent, RenderComponent, "PointRenderer")

	PointRendererComponent::PointRendererComponent() noexcept
	{
	}

	PointRendererComponent::PointRendererComponent(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	PointRendererComponent::PointRendererComponent(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	PointRendererComponent::~PointRendererComponent() noexcept
	{
	}

	GameComponentPtr
	PointRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PointRendererComponent>();
		instance->setMaterial(this->getMaterial()->clone());
		return instance;
	}

	void
	PointRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter, this);

		auto transform = this->getComponent<TransformComponent>();
		auto meshFilter = this->getComponent<MeshFilter>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setDrawType(video::DrawType::Points);
		geometry_->setActive(true);
		geometry_->setMesh(meshFilter ? meshFilter->getMesh() : nullptr);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	PointRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter, this);

		if (geometry_)
			geometry_->setActive(false);
	}

	void
	PointRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilter>())
		{
			onMeshReplaceEvent_ = std::bind(&PointRendererComponent::onMeshReplace, this, std::placeholders::_1);
			component->downcast<MeshFilter>()->addMeshListener(&onMeshReplaceEvent_);
		}
	}

	void
	PointRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilter>())
			component->downcast<MeshFilter>()->removeMeshListener(&onMeshReplaceEvent_);
	}

	void
	PointRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	PointRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	PointRendererComponent::onMeshReplace(const video::MeshPtr& mesh) noexcept
	{
		if (geometry_)
			geometry_->setMesh(mesh);
	}

	void
	PointRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}
}