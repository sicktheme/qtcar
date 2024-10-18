#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QRegularExpression>
#include <QPushButton>
#include <QComboBox>

class Car {
public:
    Car(int id, const QString &engineType, const QString &year, const QString &brand, const QString &duration, bool engineOn, bool lightsOn, bool alarmOn)
        : id(id), engineType(engineType), year(year), brand(brand), duration(duration), engineOn(engineOn), lightsOn(lightsOn), alarmOn(alarmOn) {}

    int getId() const { return id; }
    QString getEngineType() const { return engineType; }
    QString getYear() const { return year; }
    QString getBrand() const { return brand; }
    QString getDuration() const { return duration; }
    bool isEngineOn() const { return engineOn; }
    bool isLightsOn() const { return lightsOn; }
    bool isAlarmOn() const { return alarmOn; }

    void setEngineType(const QString &value) { engineType = value; }
    void setYear(const QString &value) { year = value; }
    void setBrand(const QString &value) { brand = value; }
    void setDuration(const QString &value) { duration = value; }
    void setEngineOn(bool value) { engineOn = value; }
    void setLightsOn(bool value) { lightsOn = value; }
    void setAlarmOn(bool value) { alarmOn = value; }

private:
    int id;
    QString engineType;
    QString year;
    QString brand;
    QString duration;
    bool engineOn;
    bool lightsOn;
    bool alarmOn;
};

class CarManager : public QWidget {
    Q_OBJECT

public:
    CarManager(QWidget *parent = nullptr) : QWidget(parent), currentId(1) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        tableWidget = new QTableWidget(this);
        tableWidget->setColumnCount(8);
        tableWidget->setHorizontalHeaderLabels({"ID", "Тип двигателя", "Год производства", "Марка", "Продолжительность эксплуатации", "Двигатель", "Фары", "Сигнализация"});

        // Устанавливаем режим интерактивного изменения ширины столбцов
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

        // Устанавливаем растяжение последнего столбца
        tableWidget->horizontalHeader()->setStretchLastSection(true);

        // Устанавливаем минимальную ширину для таблицы
        tableWidget->setMinimumWidth(800);

        tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked); // Позволяет редактировать ячейки двойным кликом

        // Устанавливаем стиль таблицы
        tableWidget->setStyleSheet("QTableWidget { border: 1px solid #ccc; }"
                                   "QHeaderView::section { background-color: white; border: 1px solid #ccc; color: black; font-weight: bold; }"  // Заголовок с белым фоном и черным текстом
                                   "QTableWidget::item { padding: 5px; text-align: center; color: white; background-color: #444; }"); // Цвет текста белый и фон ячеек темный

        // Создаем кнопки для управления функционалом
        QPushButton *actionButton = new QPushButton("Выбрать действие", this);
        QPushButton *resetButton = new QPushButton("Сбросить поиск", this);

        // Соединяем кнопки с методами
        connect(actionButton, &QPushButton::clicked, this, &CarManager::chooseAction);
        connect(resetButton, &QPushButton::clicked, this, &CarManager::resetSearch);

        // Добавляем кнопки в компоновщик
        layout->addWidget(tableWidget);
        layout->addWidget(actionButton);
        layout->addWidget(resetButton);

        setLayout(layout);
        setWindowTitle("Управление машинами");
    }

private slots:
    void chooseAction() {
        QStringList actions = {"Добавить машину", "Удалить последнюю машину", "Удалить машину по ID", "Поиск машины", "Поиск по ID", "Изменить данные у машины"};
        QString action = QInputDialog::getItem(this, "Выберите действие", "Действие:", actions, 0, false);

        if (action.isEmpty()) return;

        if (action == "Добавить машину") {
            addCar();
        } else if (action == "Удалить последнюю машину") {
            removeLastCar();
        } else if (action == "Удалить машину по ID") {
            removeCarById();
        } else if (action == "Поиск машины") {
            searchCar();
        } else if (action == "Поиск по ID") {
            searchCarById();
        } else if (action == "Изменить данные у машины") {
            modifyCarData();
        }
    }

    void addCar() {
        QString engineType = QInputDialog::getText(this, "Добавить машину", "Тип двигателя:");
        if (!validateEngineType(engineType)) return;

        QString year = QInputDialog::getText(this, "Добавить машину", "Год производства:");
        if (!validateYear(year)) return;

        QString brand = QInputDialog::getText(this, "Добавить машину", "Марка:");
        if (!validateBrand(brand)) return;

        QString duration = QInputDialog::getText(this, "Добавить машину", "Продолжительность эксплуатации:");
        if (!validateDuration(duration)) return;

        bool engineOn = QMessageBox::question(this, "Добавить машину", "Двигатель включен?") == QMessageBox::Yes;
        bool lightsOn = QMessageBox::question(this, "Добавить машину", "Фары включены?") == QMessageBox::Yes;
        bool alarmOn = QMessageBox::question(this, "Добавить машину", "Сигнализация включена?") == QMessageBox::Yes;

        // Создаем и добавляем машину
        cars.append(Car(currentId++, engineType, year, brand, duration, engineOn, lightsOn, alarmOn));
        displayCars();
    }

    void removeLastCar() {
        if (cars.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Нет машин для удаления.");
            return;
        }
        cars.removeLast();
        currentId--;  // Уменьшаем текущий ID
        resetSearch();
    }

    void removeCarById() {
        bool ok;
        int id = QInputDialog::getInt(this, "Удалить машину по ID", "Введите ID:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            auto it = std::remove_if(cars.begin(), cars.end(), [id](const Car &car) { return car.getId() == id; });
            if (it != cars.end()) {
                cars.erase(it, cars.end());
                resetSearch();
            } else {
                QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
            }
        }
    }

    void searchCar() {
        QString characteristic = QInputDialog::getItem(this, "Поиск по характеристикам", "Выберите характеристику:", {"Тип двигателя", "Год производства", "Марка", "Продолжительность эксплуатации", "Двигатель", "Фары", "Сигнализация"}, 0, false);
        if (characteristic.isEmpty()) return;

        QString value = QInputDialog::getText(this, "Поиск по характеристикам", "Введите значение для поиска:");
        if (value.isEmpty()) return;

        tableWidget->clearContents();
        tableWidget->setRowCount(0);  // Очищаем таблицу для результатов поиска

        for (const Car &car : cars) {
            bool match = false;

            if (characteristic == "Тип двигателя" && car.getEngineType() == value) {
                match = true;
            } else if (characteristic == "Год производства" && car.getYear() == value) {
                match = true;
            } else if (characteristic == "Марка" && car.getBrand() == value) {
                match = true;
            } else if (characteristic == "Продолжительность эксплуатации" && car.getDuration() == value) {
                match = true;
            } else if (characteristic == "Двигатель" && (car.isEngineOn() ? "Да" : "Нет") == value) {
                match = true;
            } else if (characteristic == "Фары" && (car.isLightsOn() ? "Да" : "Нет") == value) {
                match = true;
            } else if (characteristic == "Сигнализация" && (car.isAlarmOn() ? "Да" : "Нет") == value) {
                match = true;
            }

            if (match) {
                int rowCount = tableWidget->rowCount();
                tableWidget->insertRow(rowCount);
                tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
                tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
                tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
                tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
                tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
                tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
                tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
                tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
            }
        }

        if (tableWidget->rowCount() == 0) {
            QMessageBox::information(this, "Результаты поиска", "Машины с заданными характеристиками не найдены.");
        }
    }

    void searchCarById() {
        bool ok;
        int id = QInputDialog::getInt(this, "Поиск машины по ID", "Введите ID:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            tableWidget->clearContents();
            tableWidget->setRowCount(0);  // Очищаем таблицу для результатов поиска

            for (const Car &car : cars) {
                if (car.getId() == id) {
                    int rowCount = tableWidget->rowCount();
                    tableWidget->insertRow(rowCount);
                    tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
                    tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
                    tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
                    tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
                    tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
                    tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
                    tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
                    tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
                    return;
                }
            }
            QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
        }
    }

    void resetSearch() {
        tableWidget->clearContents();
        displayCars();
    }

    void displayCars() {
        tableWidget->setRowCount(0);  // Очищаем таблицу перед отображением

        for (const Car &car : cars) {
            int rowCount = tableWidget->rowCount();
            tableWidget->insertRow(rowCount);
            tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
            tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
            tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
            tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
            tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
            tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
            tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
            tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
        }
    }

    void modifyCarData() {
        bool ok;
        int id = QInputDialog::getInt(this, "Изменить данные машины", "Введите ID машины:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            for (Car &car : cars) {
                if (car.getId() == id) {
                    QString engineType = QInputDialog::getText(this, "Изменить данные", "Тип двигателя:", QLineEdit::Normal, car.getEngineType());
                    if (!validateEngineType(engineType)) return;

                    QString year = QInputDialog::getText(this, "Изменить данные", "Год производства:", QLineEdit::Normal, car.getYear());
                    if (!validateYear(year)) return;

                    QString brand = QInputDialog::getText(this, "Изменить данные", "Марка:", QLineEdit::Normal, car.getBrand());
                    if (!validateBrand(brand)) return;

                    QString duration = QInputDialog::getText(this, "Изменить данные", "Продолжительность эксплуатации:", QLineEdit::Normal, car.getDuration());
                    if (!validateDuration(duration)) return;

                    bool engineOn = QMessageBox::question(this, "Изменить данные", "Двигатель включен?") == QMessageBox::Yes;
                    bool lightsOn = QMessageBox::question(this, "Изменить данные", "Фары включены?") == QMessageBox::Yes;
                    bool alarmOn = QMessageBox::question(this, "Изменить данные", "Сигнализация включена?") == QMessageBox::Yes;

                    // Обновляем данные машины
                    car.setEngineType(engineType);
                    car.setYear(year);
                    car.setBrand(brand);
                    car.setDuration(duration);
                    car.setEngineOn(engineOn);
                    car.setLightsOn(lightsOn);
                    car.setAlarmOn(alarmOn);

                    resetSearch(); // Обновляем отображение таблицы
                    return;
                }
            }
            QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
        }
    }

private:
    QTableWidget *tableWidget;
    QList<Car> cars;
    int currentId;

    bool validateEngineType(const QString &engineType) {
        if (engineType.isEmpty() || QRegularExpression("[0-9]+").match(engineType).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Тип двигателя не должен содержать цифры.");
            return false;
        }
        return true;
    }

    bool validateYear(const QString &year) {
        if (year.isEmpty() || !QRegularExpression("[0-9]+").match(year).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Год производства должен содержать только цифры.");
            return false;
        }
        return true;
    }

    bool validateBrand(const QString &brand) {
        if (brand.isEmpty() || QRegularExpression("[0-9]+").match(brand).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Марка не должна содержать цифры.");
            return false;
        }
        return true;
    }

    bool validateDuration(const QString &duration) {
        if (duration.isEmpty() || !QRegularExpression("[0-9]+").match(duration).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Продолжительность эксплуатации должна содержать только цифры.");
            return false;
        }
        return true;
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    CarManager w;
    w.resize(800, 600); // Устанавливаем размер окна
    w.show();
    return a.exec();
}

#include "main.moc"
