#include "google_test.h"
#include "DesignerScene.h"
#include "item_constants.h"
#include "LayerItems.h"
#include "LayerView.h"
#include "NodeEditorConnection.h"
#include "ParticleLayoutItem.h"
#include "modelutils.h"
#include "MultiLayerView.h"
#include "SampleModel.h"

using namespace ModelView;

class TestScene : public ::testing::Test
{
public:
    TestScene();
    ~TestScene() override;

protected:
    QRectF makeFrame(QRectF bound);
    QRectF selectSpaceBetween(const QRectF& left, const QRectF& right);

    SampleModel model;
    std::unique_ptr<DesignerScene> scene;
};

TestScene::TestScene()
    : scene(std::make_unique<DesignerScene>(&model))
{
    model.clear();
}

TestScene::~TestScene() = default;

QRectF TestScene::makeFrame(QRectF bound)
{
    return QRectF(bound.left() - 0.1, bound.top() - 0.1, bound.width() + 0.2, bound.height() + 0.2);
}

QRectF TestScene::selectSpaceBetween(const QRectF& left, const QRectF& right)
{
    const qreal x_left = left.right() + 0.1;
    const qreal y_top = std::max(left.top(), right.top());
    const qreal width = right.left() - left.right() - 0.2;
    const qreal height = std::max(left.height(), right.height());
    return QRectF(x_left, y_top, width, height);
}

TEST_F(TestScene, testEmptyScene)
{
    auto views = scene->views();
    EXPECT_TRUE(views.empty());
}

TEST_F(TestScene, testSelection)
{
    auto mlayer = model.insertItem<MultiLayerItem>();
    auto layer = model.insertItem<LayerItem>(mlayer, MultiLayerItem::T_LAYERS);
    auto mview = dynamic_cast<QGraphicsItem*>(scene->getViewForItem(mlayer));
    auto lview = dynamic_cast<QGraphicsItem*>(scene->getViewForItem(layer));

    EXPECT_TRUE(mview != nullptr);
    EXPECT_TRUE(lview != nullptr);

    QPainterPath selection_path;
    selection_path.addRect(mview->sceneBoundingRect());
    scene->setSelectionArea(selection_path);

    EXPECT_TRUE(scene->selectedItems().contains(mview));
    EXPECT_TRUE(scene->selectedItems().contains(lview));

    scene->clearSelection();
    EXPECT_FALSE(scene->selectedItems().contains(mview));
    EXPECT_FALSE(scene->selectedItems().contains(lview));

    selection_path = QPainterPath();
    selection_path.addRect(makeFrame(lview->sceneBoundingRect()));
    scene->setSelectionArea(selection_path, Qt::ContainsItemShape);

    EXPECT_FALSE(scene->selectedItems().contains(mview));
    EXPECT_TRUE(scene->selectedItems().contains(lview));
}

TEST_F(TestScene, testAddConnection)
{
    const auto mlayer = model.insertItem<MultiLayerItem>();
    const auto layer = model.insertItem<LayerItem>(mlayer, MultiLayerItem::T_LAYERS);
    const auto layout = model.insertItem<ParticleLayoutItem>();

    EXPECT_TRUE(layout->parent() == model.rootItem());

    auto connection = new NodeEditorConnection(scene.get());
    connection->setPort1(
        dynamic_cast<ConnectableView*>(scene->getViewForItem(layout))->getOutputPorts()[0]);
    connection->setPort2(
        dynamic_cast<ConnectableView*>(scene->getViewForItem(layer))->getInputPorts()[0]);

    scene->onConnect(connection);

    EXPECT_TRUE(layout->parent() == layer);
    EXPECT_TRUE(scene->getViewForItem(layout)->parentObject() != scene->getViewForItem(layer));
}

TEST_F(TestScene, testDeleteLayer)
{
    const auto mlayer = model.insertItem<MultiLayerItem>();
    const std::string mid = mlayer->identifier();
    const auto layer = model.insertItem<LayerItem>(mlayer, MultiLayerItem::T_LAYERS);
    const std::string lid = layer->identifier();

    QPainterPath selection_path;
    selection_path.addRect(makeFrame(scene->getViewForItem(layer)->sceneBoundingRect()));
    scene->setSelectionArea(selection_path, Qt::ContainsItemShape);

    scene->deleteSelectedItems();

    EXPECT_TRUE(model.findItem(mid) == mlayer);
    EXPECT_FALSE(model.findItem(lid));
    EXPECT_TRUE(scene->getViewForItem(mlayer));
    EXPECT_FALSE(scene->getViewForItem(layer));
}

TEST_F(TestScene, testDeleteConnection)
{
    const auto mlayer = model.insertItem<MultiLayerItem>();
    const std::string mid = mlayer->identifier();
    const auto layer = model.insertItem<LayerItem>(mlayer, MultiLayerItem::T_LAYERS);
    const std::string lid = layer->identifier();
    const auto layout = model.insertItem<ParticleLayoutItem>(layer, LayerItem::T_LAYOUTS);
    const std::string layout_id = layout->identifier();

    auto layout_view = scene->getViewForItem(layout);
    layout_view->moveBy(-300.0, 0.0); // move layout to make capturing the connection simpler

    auto m_view = scene->getViewForItem(mlayer);

    // selecting the connection between the layout and the layer
    QPainterPath selection_path;
    selection_path.addRect(
        selectSpaceBetween(layout_view->sceneBoundingRect(), m_view->sceneBoundingRect()));
    scene->setSelectionArea(selection_path, Qt::IntersectsItemShape);

    auto selected = scene->selectedItems();
    EXPECT_FALSE(selected.isEmpty());
    NodeEditorConnection* connection =
        selected.isEmpty() ? nullptr : dynamic_cast<NodeEditorConnection*>(selected.front());
    EXPECT_TRUE(connection);

    scene->deleteSelectedItems();

    selected = scene->selectedItems();
    EXPECT_TRUE(selected.isEmpty());

    EXPECT_TRUE(model.findItem(mid) == mlayer);
    EXPECT_TRUE(model.findItem(lid) == layer);
    EXPECT_TRUE(model.findItem(layout_id) == layout);
    EXPECT_TRUE(layout->parent() == model.rootItem());
    EXPECT_TRUE(scene->getViewForItem(mlayer));
    EXPECT_TRUE(scene->getViewForItem(layer));
    EXPECT_TRUE(scene->getViewForItem(layout));
}

TEST_F(TestScene, testDeleteMultilayer)
{
    const auto mlayer = model.insertItem<MultiLayerItem>();
    const std::string mid = mlayer->identifier();
    const auto layer = model.insertItem<LayerItem>(mlayer, MultiLayerItem::T_LAYERS);
    const std::string lid = layer->identifier();
    const auto layout = model.insertItem<ParticleLayoutItem>(layer, LayerItem::T_LAYOUTS);
    const std::string layout_id = layout->identifier();

    auto layout_view = scene->getViewForItem(layout);
    layout_view->moveBy(-300.0, 0.0); // move layout to make capturing the connection simpler

    auto m_view = scene->getViewForItem(mlayer);

    // selecting the multilayer (layer stays unselected)
    QPainterPath selection_path;
    QRectF selection_rect;
    selection_rect.setX(m_view->sceneBoundingRect().right() - 5.0);
    selection_rect.setY(m_view->sceneBoundingRect().top() - 5.0);
    selection_rect.setSize({10.0, 10.0});
    selection_path.addRect(selection_rect);
    scene->setSelectionArea(selection_path, Qt::IntersectsItemBoundingRect);

    auto selected = scene->selectedItems();
    EXPECT_FALSE(selected.isEmpty());
    MultiLayerView* multilayer_view =
        selected.isEmpty() ? nullptr : dynamic_cast<MultiLayerView*>(selected.front());
    EXPECT_TRUE(multilayer_view == m_view);

    scene->deleteSelectedItems();

    selected = scene->selectedItems();
    EXPECT_TRUE(selected.isEmpty());

    EXPECT_FALSE(model.findItem(mid));
    EXPECT_FALSE(model.findItem(lid));
    EXPECT_TRUE(model.findItem(layout_id) == layout);
    EXPECT_TRUE(layout->parent() == model.rootItem());
    EXPECT_FALSE(scene->getViewForItem(mlayer));
    EXPECT_FALSE(scene->getViewForItem(layer));
    EXPECT_TRUE(scene->getViewForItem(layout));
}
