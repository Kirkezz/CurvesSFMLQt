#include "spacegui.h"
#include "./ui_spacegui.h"
#include <QColorDialog>
#include <QFileDialog>

using namespace std;
SpaceGUI::SpaceGUI(Space& space, SpaceRenderer& renderer, QWidget* parent) : QMainWindow(parent), ui(new Ui::SpaceGUI), space(space), renderer(renderer) {
    ui->setupUi(this);
    ui->checkBox_2->setChecked(true);
    ui->checkBox_3->setChecked(true);
    ui->checkBox_4->setChecked(true);
    ui->checkBox_5->setChecked(true);
    ui->checkBox_6->setChecked(true);
    loadCD(CustomizationData());
}
SpaceGUI::~SpaceGUI() { delete ui; }
void SpaceGUI::on_tiling_currentIndexChanged(int index) {
    if(index > 1) { // Space::AMORPHOUS
        ui->label->setText(QString("Ячеек"));
        ui->SParam1->setValue(256);
        ui->label_2->setText(QString("Сглаживание"));
        ui->SParam2->setValue(3);
        ui->label_4->setText(QString("Ширина окна"));
        ui->SRWidth->setValue(768);
        ui->label_5->setText(QString("Высота окна"));
        ui->SRHeight->setValue(768);
    } else if(prevTilingIndex > 1) {
        ui->label->setText(QString("Столбцов"));
        ui->SParam2->setValue(16);
        ui->label_2->setText(QString("Строк"));
        ui->SParam1->setValue(16);
        ui->label_4->setText(QString("Ширина ячейки"));
        ui->SRWidth->setValue(48);
        ui->label_5->setText(QString("Высота ячейки"));
        ui->SRHeight->setValue(48);
    }
    ui->SParam2->setMinimum(index < 2);
    prevTilingIndex = index;
}
void SpaceGUI::on_apply_clicked() {
    switch(ui->tiling->currentIndex()) {
    case 0:
        space.resize(ui->SParam1->value(), ui->SParam2->value(), Space::SQUARE);
        break;
    case 1:
        space.resize(ui->SParam1->value(), ui->SParam2->value(), Space::HEXAGON);
        break;
    case 2: // Space::AMORPHOUS (Voronoi)
        space.VoronoiPoints.clear();
        space.VoronoiPoints.reserve(ui->SParam1->value());
        for(size_t i = 0; i < ui->SParam1->value(); ++i) {
            space.VoronoiPoints.push_back(
                {uniform_int_distribution(0, ui->SRHeight->value())(space.dre), uniform_int_distribution(0, ui->SRWidth->value())(space.dre)});
        }
        space.resize(Point2Df(ui->SRWidth->value(), ui->SRHeight->value()), ui->SParam2->value());
        break;
    }
    renderer.path.clear();
    space.fillStepList.clear();
    space.pathStepList.clear();
    space.stepByStepFill = ui->isSBSF->isChecked();
    renderer.spaceFillStepListDelay = ui->mazeAlgDelay->value();
    renderer.manualFillStep = ui->isManualMazeStep->isChecked();
    (space.*(space.fillArr[ui->mazeAlg->currentIndex()]))();
    if(space.stepByStepFill) {
        space.stepByStepFill = false;
        space.clear();
        space.stepByStepFill = true;
    }
    renderer.loadSpaceProps(Point2Df(ui->SRWidth->value(), ui->SRHeight->value()));
}
void SpaceGUI::on_isSBSF_stateChanged(int arg1) {
    ui->mazeAlgDelay->setEnabled(arg1 && !ui->isManualMazeStep->isChecked());
    ui->isManualMazeStep->setEnabled(arg1);
}
void SpaceGUI::on_isManualMazeStep_stateChanged(int arg1) { ui->mazeAlgDelay->setEnabled(!arg1 && ui->isSBSF->isChecked()); }
void SpaceGUI::on_checkBox_2_stateChanged(int arg1) { renderer.isGridDrawn = arg1; }
void SpaceGUI::on_checkBox_3_stateChanged(int arg1) { renderer.isPathDrawn = arg1; }
void SpaceGUI::on_checkBox_4_stateChanged(int arg1) { renderer.isMazeDrawn = arg1; }
void SpaceGUI::on_checkBox_5_stateChanged(int arg1) { renderer.isCurveDrawn = arg1; }
void SpaceGUI::on_checkBox_6_stateChanged(int arg1) { renderer.isPointsDrawn = arg1; }
void SpaceGUI::on_checkBox_7_stateChanged(int arg1) { renderer.isDebugInfoDrawn = arg1; }
void SpaceGUI::on_pathAlg_currentIndexChanged(int index) { renderer.selectedPathAlg = index; }
void SpaceGUI::on_isPathAlgVisualization_stateChanged(int arg1) {
    space.stepByStepPath = arg1;
    ui->pathAlgDelay->setEnabled(arg1 && !ui->isManualPathStep->isChecked());
    ui->isManualPathStep->setEnabled(arg1);
}
void SpaceGUI::on_isManualPathStep_stateChanged(int arg1) {
    renderer.manualPathStep = arg1;
    ui->pathAlgDelay->setEnabled(!arg1 && ui->isPathAlgVisualization->isChecked());
}
void SpaceGUI::on_pathAlgDelay_valueChanged(double arg1) { renderer.spacePathStepListDelay = arg1; }
void SpaceGUI::selectColor(QFrame* frame, sf::Color& color, QColor qc) {
    frame->setStyleSheet(QString("background-color: %1;").arg(qc.name()));
    color = sf::Color(qc.red(), qc.green(), qc.blue(), qc.alpha());
}
void SpaceGUI::on_backgroundColorButton_clicked() { selectColor(ui->backgroundColorFrame, renderer.colorScheme[SpaceRenderer::BACKGROUNDCOLOR]); }
void SpaceGUI::on_outlineColorButton_clicked() { selectColor(ui->outlineColorFrame, renderer.colorScheme[SpaceRenderer::OUTLINECOLOR]); }
void SpaceGUI::on_curveColorButton_clicked() { selectColor(ui->curveColorFrame, renderer.colorScheme[SpaceRenderer::CURVECOLOR]); }
void SpaceGUI::on_mazeColorButton_clicked() { selectColor(ui->mazeColorFrame, renderer.colorScheme[SpaceRenderer::MAZECOLOR]); }
void SpaceGUI::on_pointsColorButton_clicked() { selectColor(ui->pointsColorFrame, renderer.colorScheme[SpaceRenderer::POINTSCOLOR]); }
void SpaceGUI::on_pathColorButton_clicked() { selectColor(ui->pathColorFrame, renderer.colorScheme[SpaceRenderer::PATHCOLOR]); }
void SpaceGUI::on_shapeColorButton_clicked() { selectColor(ui->shapeColorFrame, renderer.colorScheme[SpaceRenderer::DEFAULTCOLOR]); }
void SpaceGUI::on_firstColorButton_clicked() { selectColor(ui->firstColorFrame, renderer.colorScheme[SpaceRenderer::COLOR1]); }
void SpaceGUI::on_secondColorButton_clicked() { selectColor(ui->secondColorFrame, renderer.colorScheme[SpaceRenderer::COLOR2]); }
void SpaceGUI::on_thirdColorButton_clicked() { selectColor(ui->thirdColorFrame, renderer.colorScheme[SpaceRenderer::COLOR3]); }
void SpaceGUI::on_fourthColorButton_clicked() { selectColor(ui->fourthColorFrame, renderer.colorScheme[SpaceRenderer::COLOR4]); }
void SpaceGUI::on_outlineThickness_valueChanged(double arg1) { renderer.outlineThickness = arg1; }
void SpaceGUI::on_curveThickness_valueChanged(double arg1) { renderer.curveThickness = arg1; }
void SpaceGUI::on_pathThickness_valueChanged(double arg1) { renderer.pathThickness = arg1; }
void SpaceGUI::on_mazeThickness_valueChanged(double arg1) { renderer.mazeThickness = arg1; }
void SpaceGUI::on_saveToFileButton_clicked() {
    string filename = QFileDialog::getSaveFileName(this, tr("Назовите файл кастомизации"), "", tr("Двоичные файлы (*.bin)")).toStdString();
    ofstream fs(filename, ios::binary | ios::trunc | ios::out);
    CustomizationData cd;
    for(size_t i = 0; i < SpaceRenderer::NUM_COLORS; ++i) {
        cd.colors[i] = renderer.colorScheme[i];
    }
    cd.thicknesses = {renderer.outlineThickness, renderer.curveThickness, renderer.pathThickness, renderer.mazeThickness};
    fs.write((char*)&cd, sizeof(cd));
    fs.close();
}
void SpaceGUI::on_loadFromFileButton_clicked() {
    string filename = QFileDialog::getOpenFileName(this, tr("Выберите файл кастомизации"), "", tr("Двоичные файлы (*.bin)")).toStdString();
    ifstream fs(filename, ios::binary | ios::in);
    CustomizationData cd;
    fs.read((char*)&cd, sizeof(cd));
    loadCD(cd);
    fs.close();
}
void SpaceGUI::loadCD(const CustomizationData& cd) {
    auto temp_cast = [](sf::Color c) { return QColor(c.r, c.g, c.b, c.a); };
    selectColor(ui->shapeColorFrame, renderer.colorScheme[SpaceRenderer::DEFAULTCOLOR], temp_cast(cd.colors[0]));
    selectColor(ui->firstColorFrame, renderer.colorScheme[SpaceRenderer::COLOR1], temp_cast(cd.colors[1]));
    selectColor(ui->secondColorFrame, renderer.colorScheme[SpaceRenderer::COLOR2], temp_cast(cd.colors[2]));
    selectColor(ui->thirdColorFrame, renderer.colorScheme[SpaceRenderer::COLOR3], temp_cast(cd.colors[3]));
    selectColor(ui->fourthColorFrame, renderer.colorScheme[SpaceRenderer::COLOR4], temp_cast(cd.colors[4]));
    selectColor(ui->outlineColorFrame, renderer.colorScheme[SpaceRenderer::OUTLINECOLOR], temp_cast(cd.colors[5]));
    selectColor(ui->curveColorFrame, renderer.colorScheme[SpaceRenderer::CURVECOLOR], temp_cast(cd.colors[6]));
    selectColor(ui->pathColorFrame, renderer.colorScheme[SpaceRenderer::PATHCOLOR], temp_cast(cd.colors[7]));
    selectColor(ui->mazeColorFrame, renderer.colorScheme[SpaceRenderer::MAZECOLOR], temp_cast(cd.colors[8]));
    selectColor(ui->backgroundColorFrame, renderer.colorScheme[SpaceRenderer::BACKGROUNDCOLOR], temp_cast(cd.colors[9]));
    selectColor(ui->pointsColorFrame, renderer.colorScheme[SpaceRenderer::POINTSCOLOR], temp_cast(cd.colors[10]));
    ui->outlineThickness->setValue(cd.thicknesses[0]);
    ui->curveThickness->setValue(cd.thicknesses[1]);
    ui->pathThickness->setValue(cd.thicknesses[2]);
    ui->mazeThickness->setValue(cd.thicknesses[3]);
}
