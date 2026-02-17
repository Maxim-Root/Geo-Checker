#include "mainwindow.hpp"
#include "lang_ui_config.hpp"
#include <QtConcurrent>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QFutureWatcher>
#include <QGridLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QStyleHints>
#endif
#ifdef Q_OS_WIN
#include <QSettings>
#endif
#include <algorithm>
#include <future>
#include <set>
#include <thread>

namespace geochecker {

struct DnsResult {
    int checked;
    std::vector<std::string> ips;
};

static QMap<QString, QMap<QString, QString>> translations = []() {
    QMap<QString, QMap<QString, QString>> t;
    t["ru"]["theme"] = "Тема";
    t["ru"]["theme_dark"] = "Тёмная";
    t["ru"]["theme_white"] = "Светлая";
    t["ru"]["theme_system"] = "Системная";
    t["ru"]["language"] = "Язык";
    t["ru"]["data_files"] = "Файлы данных";
    t["ru"]["path_geosite"] = "Путь к geosite.dat";
    t["ru"]["path_geoip"] = "Путь к geoip.dat";
    t["ru"]["open_geosite"] = "Открыть geosite.dat";
    t["ru"]["open_geoip"] = "Открыть geoip.dat";
    t["ru"]["stats"] = "Статистика";
    t["ru"]["copy"] = "Копировать";
    t["ru"]["save"] = "Сохранить";
    t["ru"]["search"] = "Поиск";
    t["ru"]["domain"] = "Домен";
    t["ru"]["find_categories"] = "Найти категории";
    t["ru"]["domains"] = "Домены";
    t["ru"]["ip_from_geoip"] = "IP из geoip";
    t["ru"]["results"] = "Результаты";
    t["ru"]["tab_categories"] = "Категории";
    t["ru"]["tab_domains"] = "Домены";
    t["ru"]["tab_ip"] = "IP";
    t["ru"]["ok"] = "OK";
    t["ru"]["enter_domain"] = "Введите домен для поиска";
    t["ru"]["enter_geosite_tag"] = "Введите geosite:tag";
    t["ru"]["enter_geoip_tag"] = "Введите geoip:tag";
    t["ru"]["no_results_to_copy"] = "Нет результата для копирования";
    t["ru"]["no_results_to_save"] = "Нет результата для сохранения";
    t["ru"]["copied"] = "Результат скопирован в буфер обмена";
    t["ru"]["saved_to"] = "Сохранено: %1";
    t["ru"]["set_geosite_path"] = "Укажите путь к geosite.dat";
    t["ru"]["set_geoip_path"] = "Укажите путь к geoip.dat";
    t["ru"]["file_not_found"] = "Файл не найден: %1";
    t["ru"]["loading_geosite"] = "Загружаю geosite.dat...";
    t["ru"]["loading_geoip"] = "Загружаю geoip.dat...";
    t["ru"]["load_geosite_failed"] = "Не удалось загрузить geosite: %1";
    t["ru"]["load_geoip_failed"] = "Не удалось загрузить geoip: %1";
    t["ru"]["ready"] = "Готово";
    t["ru"]["searching_categories"] = "Ищу категории домена...";
    t["ru"]["not_found"] = "Не найдено";
    t["ru"]["categories_found"] = "Найдено категорий: %1";
    t["ru"]["getting_domains"] = "Получаю домены из geosite...";
    t["ru"]["and_more"] = "... и еще %1";
    t["ru"]["domains_count"] = "Домены (%1):";
    t["ru"]["empty_list"] = "Список пуст";
    t["ru"]["domains_received"] = "Получено доменов: %1";
    t["ru"]["dns_progress"] = "DNS резолвинг: %1/%2";
    t["ru"]["dns_error"] = "Ошибка DNS резолвинга";
    t["ru"]["ips_not_found"] = "IP не найдены";
    t["ru"]["dns_done"] = "DNS готово. Найдено IP: %1";
    t["ru"]["dns_starting"] = "Запускаю DNS резолвинг... Подождите.";
    t["ru"]["dns_running"] = "Выполняю DNS резолвинг...";
    t["ru"]["getting_geoip_ranges"] = "Получаю диапазоны из geoip...";
    t["ru"]["ip_cidr_count"] = "IP/CIDR (%1):";
    t["ru"]["ranges_received"] = "Получено диапазонов: %1";
    t["ru"]["choose_geosite"] = "Выберите geosite.dat";
    t["ru"]["choose_geoip"] = "Выберите geoip.dat";
    t["ru"]["all_files"] = "Все файлы";
    t["ru"]["info"] = "Информация";
    t["ru"]["warning"] = "Предупреждение";
    t["ru"]["error"] = "Ошибка";
    for (auto it = t["ru"].begin(); it != t["ru"].end(); ++it) t["en"][it.key()] = it.key();
    t["en"]["theme"] = "Theme";
    t["en"]["theme_dark"] = "Dark";
    t["en"]["theme_white"] = "Light";
    t["en"]["theme_system"] = "System";
    t["en"]["language"] = "Language";
    t["en"]["data_files"] = "Data Files";
    t["en"]["path_geosite"] = "Path to geosite.dat";
    t["en"]["path_geoip"] = "Path to geoip.dat";
    t["en"]["open_geosite"] = "Open geosite.dat";
    t["en"]["open_geoip"] = "Open geoip.dat";
    t["en"]["stats"] = "Statistics";
    t["en"]["copy"] = "Copy";
    t["en"]["save"] = "Save";
    t["en"]["search"] = "Search";
    t["en"]["domain"] = "Domain";
    t["en"]["find_categories"] = "Find Categories";
    t["en"]["domains"] = "Domains";
    t["en"]["ip_from_geoip"] = "IP from geoip";
    t["en"]["results"] = "Results";
    t["en"]["tab_categories"] = "Categories";
    t["en"]["tab_domains"] = "Domains";
    t["en"]["tab_ip"] = "IP";
    t["en"]["choose_geosite"] = "Choose geosite.dat";
    t["en"]["choose_geoip"] = "Choose geoip.dat";
    t["en"]["all_files"] = "All files";
    t["en"]["no_results_to_copy"] = "No results to copy";
    t["en"]["no_results_to_save"] = "No results to save";
    return t;
}();

// Container for free positioning of the label and language button.
// Button is always aligned to the right edge; label moves independently along X.
class LanguageTrackWidget : public QWidget {
public:
    LanguageTrackWidget(QLabel* label, QComboBox* combo, QWidget* parent = nullptr)
        : QWidget(parent), label_(label), combo_(combo) {
        setAutoFillBackground(false);
        setAttribute(Qt::WA_TranslucentBackground);
        label_->setParent(this);
        combo_->setParent(this);
    }

    void setGap(int gap) {
        gap_ = gap;
        relayout();
    }

    void setLabelShiftX(int shift) {
        label_shift_x_ = shift;
        relayout();
    }

    void setComboShiftX(int shift) {
        combo_shift_x_ = shift;
        relayout();
    }

    void relayout() {
        if (!label_ || !combo_) return;
        const int raw_combo_x = width() - combo_->width() + combo_shift_x_;
        const int combo_x = std::clamp(raw_combo_x, 0, qMax(0, width() - combo_->width()));
        const int combo_y = (height() - combo_->height()) / 2;
        combo_->move(combo_x, combo_y);

        const int label_x = combo_x - gap_ - label_->width() + label_shift_x_;
        const int label_y = (height() - label_->height()) / 2;
        label_->move(label_x, label_y);
        label_->raise();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        relayout();
    }

private:
    QLabel* label_;
    QComboBox* combo_;
    int gap_ = 0;
    int label_shift_x_ = 0;
    int combo_shift_x_ = 0;
};

// Custom delegate: hover with position-aware rounded corners
// First item: rounded top corners; Last item: rounded bottom corners; Middle: rectangular
class ComboDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        const QRectF rect = QRectF(option.rect);
        const bool hovered = option.state & QStyle::State_MouseOver;
        const int row = index.row();
        const int total = index.model() ? index.model()->rowCount() : 1;
        const bool first = (row == 0);
        const bool last = (row == total - 1);
        if (hovered) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(0x3b, 0x82, 0xf6));
            const qreal r = 8.0;
            if (first && last) {
                painter->drawRoundedRect(rect, r, r);
            } else if (first) {
                painter->setClipRect(rect);
                painter->drawRoundedRect(rect.adjusted(0, 0, 0, r), r, r);
                painter->setClipping(false);
            } else if (last) {
                painter->setClipRect(rect);
                painter->drawRoundedRect(rect.adjusted(0, -r, 0, 0), r, r);
                painter->setClipping(false);
            } else {
                painter->drawRect(rect);
            }
        }
        painter->setPen(hovered ? QColor(Qt::white) : option.palette.color(QPalette::Text));
        painter->setFont(option.font);
        painter->drawText(rect.adjusted(10, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft,
                          index.data(Qt::DisplayRole).toString());
        painter->restore();
    }
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QSize s = QStyledItemDelegate::sizeHint(option, index);
        s.setHeight(qMax(s.height(), 34));
        return s;
    }
};

// Rounded popup frame with anti-aliased corners (no jagged mask)
class RoundedPopupFrame : public QFrame {
public:
    RoundedPopupFrame(QWidget* parent = nullptr)
        : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint) {
        setAttribute(Qt::WA_TranslucentBackground);
        setAutoFillBackground(false);
    }
    void setBgColor(const QColor& c) { bg_ = c; }
    void setBorderColor(const QColor& c) { border_ = c; }
    void setRadius(qreal r) { radius_ = r; }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        QRectF r(0.5, 0.5, width() - 1.0, height() - 1.0);
        p.setPen(QPen(border_, 1.0));
        p.setBrush(bg_);
        p.drawRoundedRect(r, radius_, radius_);
    }
private:
    QColor bg_ = QColor("#2a2d31");
    QColor border_ = QColor("#3c4149");
    qreal radius_ = 12.0;
};

class PopupListComboBox : public QComboBox {
public:
    using QComboBox::QComboBox;
    ~PopupListComboBox() override { hidePopup(); }

protected:
    void showPopup() override {
        hidePopup();

        popup_ = new RoundedPopupFrame();
        popup_->setStyleSheet("background: transparent;");
        bool dark = false;
        { QString ss = qApp->styleSheet(); dark = ss.contains("#2a2d31") || ss.contains("#1f2023"); }
        if (dark) {
            static_cast<RoundedPopupFrame*>(popup_)->setBgColor(QColor("#2a2d31"));
            static_cast<RoundedPopupFrame*>(popup_)->setBorderColor(QColor("#3c4149"));
        } else {
            static_cast<RoundedPopupFrame*>(popup_)->setBgColor(QColor("#ffffff"));
            static_cast<RoundedPopupFrame*>(popup_)->setBorderColor(QColor("#e2e8f0"));
        }
        const bool isLang = (objectName() == "langCombo");
        auto* popupLayout = new QVBoxLayout(popup_);
        if (isLang) {
            if (dark) popupLayout->setContentsMargins(3, 4, 3, 4);
            else      popupLayout->setContentsMargins(3, 4, 3, 4);
        } else {
            if (dark) popupLayout->setContentsMargins(3, 4, 3, 4);
            else      popupLayout->setContentsMargins(3, 4, 3, 3);
        }
        popupLayout->setSpacing(0);

        list_ = new QListWidget(popup_);
        list_->setObjectName("comboPopupList");
        list_->setItemDelegate(new ComboDelegate(list_));
        list_->setFrameShape(QFrame::NoFrame);
        list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list_->setSpacing(0);
        list_->setSelectionMode(QAbstractItemView::NoSelection);
        list_->setFocusPolicy(Qt::NoFocus);
        list_->setMouseTracking(true);
        for (int i = 0; i < count(); ++i) {
            list_->addItem(itemText(i));
        }
        list_->setCurrentRow(-1);
        list_->clearSelection();
        {
            QPalette pal = list_->palette();
            pal.setColor(QPalette::Highlight, Qt::transparent);
            pal.setColor(QPalette::HighlightedText, Qt::transparent);
            pal.setColor(QPalette::Base, Qt::transparent);
            pal.setColor(QPalette::Text, dark ? QColor("#f8fafc") : QColor("#0f172a"));
            list_->setPalette(pal);
            list_->viewport()->setPalette(pal);
            list_->viewport()->setAutoFillBackground(false);
        }
        list_->setStyleSheet(
            "QListWidget { background: transparent; selection-background-color: transparent; }"
            "QListWidget::item { background: transparent; }"
            "QListWidget::item:selected { background: transparent; }"
            "QListWidget::item:hover { background: transparent; }"
            "QListWidget::item:focus { background: transparent; outline: none; }"
        );
        popupLayout->addWidget(list_);

        const int rowH = std::max(34, list_->sizeHintForRow(0));
        const int popupRows = std::min(8, std::max(1, count()));
        int popupExtra;
        if (isLang) { popupExtra = dark ? 8 : 8; }
        else        { popupExtra = dark ? 8 : 7; }
        const int popupH = rowH * popupRows + popupExtra;
        const int popupW = std::max(width() + 8, list_->sizeHintForColumn(0) + 30);
        popup_->setFixedSize(popupW, popupH);

        QPoint pos = mapToGlobal(QPoint(0, height() + 4));
        if (QScreen* s = this->screen()) {
            const QRect a = s->availableGeometry();
            if (pos.x() + popupW > a.right()) pos.setX(a.right() - popupW);
            if (pos.x() < a.left()) pos.setX(a.left());
            if (pos.y() + popupH > a.bottom()) {
                pos = mapToGlobal(QPoint(0, -popupH - 4));
            }
            if (pos.y() < a.top()) pos.setY(a.top());
        }
        popup_->move(pos);
        popup_->installEventFilter(this);

        connect(list_, &QListWidget::itemClicked, popup_, [this](QListWidgetItem* item) {
            if (!list_) return;
            const int row = list_->row(item);
            if (row >= 0) setCurrentIndex(row);
            hidePopup();
        });

        popup_->show();
    }

    void hidePopup() override {
        if (closing_popup_) return;
        closing_popup_ = true;
        if (popup_) {
            popup_->removeEventFilter(this);
            popup_->hide();
            popup_->deleteLater();
            popup_ = nullptr;
            list_ = nullptr;
        }
        QComboBox::hidePopup();
        closing_popup_ = false;
    }

    bool eventFilter(QObject* watched, QEvent* event) override {
        if (watched == popup_ && (event->type() == QEvent::Hide || event->type() == QEvent::Close)) {
            hidePopup();
            return false;
        }
        return QComboBox::eventFilter(watched, event);
    }

private:
    QWidget* popup_ = nullptr;
    QListWidget* list_ = nullptr;
    bool closing_popup_ = false;
};

class FixedWidthMessageDialog : public QDialog {
public:
    FixedWidthMessageDialog(const QString& title, const QString& text, QMessageBox::Icon icon,
                            int preferredWidth, QWidget* parent = nullptr)
        : QDialog(parent) {
        setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        setModal(true);
        setWindowTitle(title);
        setWindowIcon(QIcon());

        int targetWidth = preferredWidth;
        if (QScreen* screen = this->screen()) {
            targetWidth = std::min(targetWidth, screen->availableGeometry().width() - 32);
        }
        setFixedWidth(targetWidth);

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(14, 12, 14, 12);
        root->setSpacing(10);

        auto* body = new QHBoxLayout();
        body->setContentsMargins(0, 0, 0, 0);
        body->setSpacing(10);
        auto* iconLabel = new QLabel();
        QStyle::StandardPixmap sp = QStyle::SP_MessageBoxInformation;
        if (icon == QMessageBox::Warning) sp = QStyle::SP_MessageBoxWarning;
        else if (icon == QMessageBox::Critical) sp = QStyle::SP_MessageBoxCritical;
        else if (icon == QMessageBox::Question) sp = QStyle::SP_MessageBoxQuestion;
        iconLabel->setPixmap(qApp->style()->standardIcon(sp).pixmap(48, 48));
        iconLabel->setFixedSize(56, 56);
        iconLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        auto* textLabel = new QLabel(text);
        textLabel->setWordWrap(true);
        textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        body->addWidget(iconLabel, 0, Qt::AlignTop);
        body->addWidget(textLabel, 1, Qt::AlignVCenter);
        root->addLayout(body);

        auto* footer = new QHBoxLayout();
        footer->setContentsMargins(0, 0, 0, 0);
        auto* okBtn = new QPushButton("OK");
        okBtn->setFlat(false);
        okBtn->setStyleSheet(
            "QPushButton {"
            " background-color: #3b82f6; color: white; border: none; border-radius: 8px;"
            " padding: 8px 20px; font-weight: bold;"
            "}"
            "QPushButton:hover { background-color: #2563eb; }"
        );
        okBtn->setMinimumWidth(96);
        okBtn->setDefault(true);
        footer->addStretch(1);
        footer->addWidget(okBtn, 0, Qt::AlignRight);
        root->addLayout(footer);

        connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
};

static void showMessage(QWidget* parent, const QString& title, const QString& text,
                       QMessageBox::Icon icon) {
    constexpr int kPreferredWidth = 245;
    FixedWidthMessageDialog msg(title, text, icon, kPreferredWidth, parent);
    msg.exec();
}

static bool detectSystemDarkTheme() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    auto scheme = qApp->styleHints()->colorScheme();
    if (scheme == Qt::ColorScheme::Dark) return true;
    if (scheme == Qt::ColorScheme::Light) return false;
#endif
#ifdef Q_OS_WIN
    QSettings s("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                QSettings::NativeFormat);
    int appsUseLight = s.value("AppsUseLightTheme", 1).toInt();
    return (appsUseLight == 0);
#endif
#ifdef Q_OS_LINUX
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start("gsettings", {"get", "org.gnome.desktop.interface", "color-scheme"}, QIODevice::ReadOnly);
    if (p.waitForFinished(500)) {
        QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed().toLower();
        if (out.contains("prefer-dark")) return true;
        if (out.contains("prefer-light") || out.contains("default")) return false;
    }
    p.start("gsettings", {"get", "org.gnome.desktop.interface", "gtk-theme"}, QIODevice::ReadOnly);
    if (p.waitForFinished(500)) {
        QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed().toLower();
        if (out.contains("dark")) return true;
    }
    p.start("gsettings", {"get", "org.gnome.desktop.interface", "gtk-application-prefer-dark-theme"}, QIODevice::ReadOnly);
    if (p.waitForFinished(500)) {
        QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed().toLower();
        if (out == "true") return true;
    }
#endif
    return true;
}

static QString lightStylesheet() {
    return QString::fromUtf8(R"(
        QMainWindow, QWidget {
            background-color: #e6e9ef;
            font-family: "Segoe UI", "Ubuntu", "Cantarell", "Noto Sans", "Helvetica Neue", Arial, sans-serif;
            font-size: 14px;
        }
        QFrame#sidebar, QFrame#mainArea {
            background-color: #eef1f6; border: 1px solid #cdd5df; border-radius: 16px;
        }
        QFrame#pathBlock, QFrame#queryCard {
            background-color: #e3e8ef; border: 1px solid #c7d0dc; border-radius: 12px;
            margin: 4px 0px;
        }
        QFrame#statsBlock {
            background-color: #e3e8ef; border: 1px solid #c7d0dc; border-radius: 12px;
            margin: 4px 0px;
        }
        QFrame#resultsCard {
            background-color: #e3e8ef; border: 1px solid #c7d0dc; border-radius: 12px;
        }
        QFrame#statCard {
            background-color: #e9edf3; border: none; border-radius: 12px;
            margin: 4px 6px;
        }
        QFrame#statCard QLabel#statTitle { color: #64748b; font-size: 12px; font-weight: 500; }
        QFrame#statCard QLabel#statValue { color: #1e293b; font-size: 26px; font-weight: 600; }
        QLineEdit {
            background-color: #eef2f6; border: 1px solid #94a3b8; border-radius: 8px;
            padding: 8px 12px; color: #1e293b; font-size: 14px; font-weight: 500; selection-background-color: #3b82f6;
        }
        QLineEdit:focus { border-color: #3b82f6; }
        QFrame#resultsCard QTextEdit {
            background-color: #f8fafc; border: 1px solid #d1d5db; border-radius: 8px;
            padding: 12px; color: #1e293b; font-family: "Cascadia Code", "Fira Code", "Consolas", "Monaco", monospace; font-size: 13px;
        }
        QPushButton#primary, QPushButton#secondary {
            background-color: #3b82f6; color: white; border: none; border-radius: 12px;
            padding: 10px 16px; font-size: 13px; font-weight: bold; min-height: 38px;
        }
        QPushButton#primary:hover, QPushButton#secondary:hover { background-color: #2563eb; }
        QPushButton#primary:pressed, QPushButton#secondary:pressed { background-color: #1d4ed8; }
        QPushButton#tabActive {
            background-color: #3b82f6; color: white; border: 1px solid #3b82f6; border-radius: 12px;
            font-size: 14px; font-weight: 600; min-height: 38px;
        }
        QPushButton#tabInactive {
            background-color: #e5e7eb; color: #334155; border: 1px solid #d1d5db; border-radius: 12px;
            font-size: 14px; font-weight: 500; min-height: 38px;
        }
        QPushButton#tabInactive:hover { background-color: #cbd5e1; }
        QComboBox {
            background-color: #3b82f6; color: white; border: none; border-radius: 12px;
            padding: 8px 12px; padding-right: 12px; font-size: 13px; min-width: %1px;
            border-top-left-radius: 12px; border-top-right-radius: 12px;
            border-bottom-left-radius: 12px; border-bottom-right-radius: 12px;
            outline: none;
        }
        QComboBox:hover { background-color: #2563eb; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: right; width: 0px; border: none; border-radius: 12px; border-top-right-radius: 12px; border-bottom-right-radius: 12px; }
        QComboBox::down-arrow { image: none; width: 0; height: 0; }
        QLabel {
            color: #1e293b;
            background-color: transparent;
            border: none;
        }
        QLabel#subtitle { color: #64748b; font-size: 13px; font-weight: 500; }
        QLabel#settingsLabel { font-size: 13px; font-weight: 500; color: #475569; background: transparent; border: none; margin: 0; padding: 0; }
        QGroupBox { font-weight: 600; color: #1e293b; }
        QProgressBar { border: 1px solid #c7d0dc; border-radius: 6px; text-align: center; }
        QProgressBar::chunk { background-color: #3b82f6; border-radius: 5px; }
        QLabel#statusLabel { color: #64748b; font-size: 13px; font-weight: 500; }
        QMessageBox, QDialog {
            background-color: #eef1f6; color: #0f172a;
        }
        QMessageBox QLabel { color: #0f172a; }
        QMessageBox QPushButton {
            background-color: #3b82f6; color: white; border: none; border-radius: 8px;
            padding: 8px 20px; font-weight: bold;
            image: none; text-decoration: none;
        }
        QMessageBox QPushButton:hover { background-color: #2563eb; }
        QFileDialog { background-color: #eef1f6; color: #0f172a; }
        QFileDialog QLabel { color: #0f172a; }
    )").arg(lang_ui::COMBO_MIN_WIDTH);
}

static QString darkStylesheet() {
    return QString::fromUtf8(R"(
        QMainWindow, QWidget { background-color: #1f2023; }
        QFrame#sidebar, QFrame#mainArea {
            background-color: #232529; border: 1px solid #343840; border-radius: 16px;
        }
        QFrame#pathBlock, QFrame#queryCard {
            background-color: #2a2d31; border: 1px solid #3c4149; border-radius: 12px;
            margin: 4px 0px;
        }
        QFrame#statsBlock {
            background-color: #30343c; border: 1px solid #454c58; border-radius: 12px;
            margin: 4px 0px;
        }
        QFrame#resultsCard {
            background-color: #2f333b; border: 1px solid #444b57; border-radius: 12px;
        }
        QFrame#statCard {
            background-color: #2b2f36; border: none; border-radius: 12px;
            margin: 4px 6px;
        }
        QFrame#statCard QLabel#statTitle { color: #9da1a6; font-size: 12px; }
        QFrame#statCard QLabel#statValue { color: #ffffff; font-size: 26px; font-weight: 600; }
        QLineEdit {
            background-color: #32363c; border: 1px solid #5c6573; border-radius: 8px;
            padding: 8px 12px; color: #f8fafc; font-size: 13px; selection-background-color: #3b82f6;
        }
        QLineEdit:focus { border-color: #3b82f6; }
        QFrame#resultsCard QTextEdit {
            background-color: #2c3037; border: 1px solid #3f4652; border-radius: 8px;
            padding: 12px; color: #e2e8f0; font-family: Consolas, monospace; font-size: 13px;
        }
        QPushButton#primary, QPushButton#secondary {
            background-color: #3b82f6; color: white; border: none; border-radius: 12px;
            padding: 10px 16px; font-size: 13px; font-weight: bold; min-height: 38px;
        }
        QPushButton#primary:hover, QPushButton#secondary:hover { background-color: #2563eb; }
        QPushButton#primary:pressed, QPushButton#secondary:pressed { background-color: #1d4ed8; }
        QPushButton#tabActive {
            background-color: #3b82f6; color: white; border: 1px solid #3b82f6; border-radius: 12px;
            font-size: 13px; font-weight: bold; min-height: 38px;
        }
        QPushButton#tabInactive {
            background-color: #1b2028; color: #9aa7bc; border: 1px solid #2a313b; border-radius: 12px;
            font-size: 13px; font-weight: bold; min-height: 38px;
        }
        QPushButton#tabInactive:hover { background-color: #252c36; }
        QComboBox {
            background-color: #3b82f6; color: white; border: none; border-radius: 12px;
            padding: 8px 12px; padding-right: 12px; font-size: 13px; min-width: %1px;
            border-top-left-radius: 12px; border-top-right-radius: 12px;
            border-bottom-left-radius: 12px; border-bottom-right-radius: 12px;
            outline: none;
        }
        QComboBox:hover { background-color: #2563eb; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: right; width: 0px; border: none; border-radius: 12px; border-top-right-radius: 12px; border-bottom-right-radius: 12px; }
        QComboBox::down-arrow { image: none; width: 0; height: 0; }
        QLabel {
            color: #f8fafc;
            background-color: transparent;
            border: none;
        }
        QLabel#subtitle { color: #94a3b8; font-size: 13px; }
        QLabel#settingsLabel { font-size: 13px; font-weight: 500; color: #a6a8ad; background: transparent; border: none; margin: 0; padding: 0; }
        QGroupBox { font-weight: bold; color: #f8fafc; }
        QProgressBar { border: 1px solid #3c4149; border-radius: 6px; text-align: center; }
        QProgressBar::chunk { background-color: #3b82f6; border-radius: 5px; }
        QLabel#statusLabel { color: #9ea4ae; }
        QMessageBox, QDialog {
            background-color: #232529; color: #f8fafc;
        }
        QMessageBox QLabel { color: #f8fafc; }
        QMessageBox QPushButton {
            background-color: #3b82f6; color: white; border: none; border-radius: 8px;
            padding: 8px 20px; font-weight: bold;
            image: none; text-decoration: none;
        }
        QMessageBox QPushButton:hover { background-color: #2563eb; }
        QFileDialog { background-color: #232529; color: #f8fafc; }
        QFileDialog QLabel { color: #f8fafc; }
    )").arg(lang_ui::COMBO_MIN_WIDTH);
}

// Logging disabled (no-op). For debugging: cmake -DGEOCHECKER_DEBUG=ON ..
static inline void geoLog(const char* /*topic*/, const QString& /*msg*/) {}

static QString detectSystemLanguage() {
    QLocale locale = QLocale::system();
    QStringList uiLangs = locale.uiLanguages();
    for (const QString& tag : uiLangs) {
        QString code = tag.split('-').first().split('_').first().toLower();
        if (code == "ru") return "ru";
        if (code == "en") return "en";
    }
    QString code = locale.name().split('_').first().toLower();
    if (code == "ru") return "ru";
    if (code == "en") return "en";
    return "en";
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , lang_(detectSystemLanguage())
{
    geoLog("MainWindow", "ctor start lang_=" + lang_);
    setWindowTitle("Geo Checker");
#ifdef GEOCHECKER_HAS_APP_ICON
    setWindowIcon(QIcon(":/icons/app_icon.ico"));
#endif
    resize(1320, 860);
    setMinimumSize(1320, 860);
    if (QScreen* screen = QGuiApplication::primaryScreen()) {
        QRect geo = screen->availableGeometry();
        move(geo.x() + (geo.width() - width()) / 2,
             geo.y() + (geo.height() - height()) / 2);
    }
    buildUI();
    theme_combo_->blockSignals(true);
    theme_combo_->setCurrentIndex(theme_combo_->findData("system"));
    theme_combo_->blockSignals(false);
    geoLog("MainWindow", "before updateThemeFromSystem");
    updateThemeFromSystem();
    // Process events before recalc so first applyLanguage uses lw from applied styles (font); removes initial 11px gap.
    QApplication::processEvents(QEventLoop::ProcessEventsFlag::AllEvents, 50);
    loadDefaultPaths();
    applyLanguage();
    geoLog("MainWindow", "ctor end");
}

MainWindow::~MainWindow() {
    removeSystemThemeListener();
}

QString MainWindow::trKey(const char* key) const {
    QString k(key);
    if (translations[lang_].contains(k)) return translations[lang_][k];
    if (translations["en"].contains(k)) return translations["en"][k];
    return k;
}

void MainWindow::refreshSettingsRowStyles() {
    QStyle* s = style();
    if (theme_label_) { s->unpolish(theme_label_); s->polish(theme_label_); }
    if (theme_combo_) { s->unpolish(theme_combo_); s->polish(theme_combo_); }
    if (language_label_) { s->unpolish(language_label_); s->polish(language_label_); }
    if (language_combo_) { s->unpolish(language_combo_); s->polish(language_combo_); }
    if (language_track_) { s->unpolish(language_track_); s->polish(language_track_); }
    QApplication::processEvents(QEventLoop::ProcessEventsFlag::AllEvents, 30);
    if (language_label_ && language_combo_ && language_track_) applyLanguageRowSizes();
}

void MainWindow::applyTheme(bool dark) {
    geoLog("applyTheme", "dark=" + QString::number(dark));
    dark_theme_ = dark;
    qApp->setStyleSheet(dark ? darkStylesheet() : lightStylesheet());
    QTimer::singleShot(0, this, [this]() {
        geoLog("applyTheme", "singleShot: refreshSettingsRowStyles");
        refreshSettingsRowStyles();
        QTimer::singleShot(30, this, [this]() {
            if (language_label_ && language_combo_ && language_track_) applyLanguageRowSizes();
        });
    });
}

bool MainWindow::isSystemDarkTheme() const {
    return detectSystemDarkTheme();
}

void MainWindow::updateThemeFromSystem() {
    bool dark = isSystemDarkTheme();
    geoLog("updateThemeFromSystem", "dark=" + QString::number(dark));
    applyTheme(dark);
    setupSystemThemeListener();
}

void MainWindow::setupSystemThemeListener() {
    removeSystemThemeListener();
    if (theme_combo_->currentData().toString() != "system") return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    system_theme_connection_ = connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
        this, [this](Qt::ColorScheme) { onSystemThemeChanged(); });
#else
    if (!system_theme_timer_) {
        system_theme_timer_ = new QTimer(this);
        connect(system_theme_timer_, &QTimer::timeout, this, &MainWindow::onSystemThemeChanged);
    }
    system_theme_timer_->start(10);
#endif
}

void MainWindow::removeSystemThemeListener() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    disconnect(system_theme_connection_);
#else
    if (system_theme_timer_) system_theme_timer_->stop();
#endif
}

void MainWindow::onSystemThemeChanged() {
    if (theme_combo_->currentData().toString() != "system") return;
    bool dark = isSystemDarkTheme();
    if (dark != dark_theme_) applyTheme(dark);
}

void MainWindow::refreshTabStyles() {
    QString active = "QPushButton#tabActive", inactive = "QPushButton#tabInactive";
    tab_cat_btn_->setObjectName(active_tab_ == 0 ? "tabActive" : "tabInactive");
    tab_dom_btn_->setObjectName(active_tab_ == 1 ? "tabActive" : "tabInactive");
    tab_ip_btn_->setObjectName(active_tab_ == 2 ? "tabActive" : "tabInactive");
    tab_cat_btn_->style()->unpolish(tab_cat_btn_);
    tab_cat_btn_->style()->polish(tab_cat_btn_);
    tab_dom_btn_->style()->unpolish(tab_dom_btn_);
    tab_dom_btn_->style()->polish(tab_dom_btn_);
    tab_ip_btn_->style()->unpolish(tab_ip_btn_);
    tab_ip_btn_->style()->polish(tab_ip_btn_);
}

void MainWindow::buildUI() {
    auto* central = new QWidget(this);
    central->setObjectName("central");
    setCentralWidget(central);
    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    auto* sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setMinimumWidth(380);
    sidebar->setMaximumWidth(420);
    auto* sl = new QVBoxLayout(sidebar);
    sl->setSpacing(12);
    sl->setContentsMargins(16, 16, 16, 16);

    QLabel* title = new QLabel("Geo Checker");
    title->setStyleSheet("font-size: 26px; font-weight: 600; background: transparent; border: none;");
    sl->addWidget(title);
    QLabel* subtitle = new QLabel("Smart Routing");
    subtitle->setObjectName("subtitle");
    sl->addWidget(subtitle);

    auto* themeRow = new QHBoxLayout;
    themeRow->setSpacing(8);
    themeRow->setAlignment(Qt::AlignVCenter);
    theme_label_ = new QLabel(trKey("theme"));
    theme_label_->setObjectName("settingsLabel");
    theme_label_->setMinimumHeight(36);
    theme_label_->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    themeRow->addWidget(theme_label_, 0, Qt::AlignVCenter);
    theme_combo_ = new PopupListComboBox;
    theme_combo_->setObjectName("themeCombo");
    theme_combo_->setItemDelegate(new ComboDelegate(theme_combo_));
    theme_combo_->addItem(trKey("theme_dark"), "dark");
    theme_combo_->addItem(trKey("theme_white"), "white");
    theme_combo_->addItem(trKey("theme_system"), "system");
    theme_combo_->setCurrentIndex(theme_combo_->findData("system"));
    const int theme_combo_w = (lang_ == "ru") ? 140 : 115;
    theme_combo_->setFixedSize(theme_combo_w, 40);
    theme_combo_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    geoLog("buildUI", "theme_combo_w=" + QString::number(theme_combo_w) + " lang_=" + lang_);
    connect(theme_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onThemeChange);
    themeRow->addWidget(theme_combo_, 0, Qt::AlignVCenter);
    // --- Language block: spacing, label, button ---
    const int lang_btn_width = (lang_ == "ru") ? lang_ui::BTN_WIDTH_RU : lang_ui::BTN_WIDTH_EN;
    const int lang_btn_height = lang_ui::BTN_HEIGHT;
    const int lang_label_width = (lang_ == "ru") ? lang_ui::LABEL_WIDTH_RU : lang_ui::LABEL_WIDTH_EN;
    lang_spacer_ = new QWidget();
    lang_spacer_->setAutoFillBackground(false);
    lang_spacer_->setAttribute(Qt::WA_TranslucentBackground);
    lang_spacer_->setFixedWidth(0);
    language_label_ = new QLabel(trKey("language"));
    language_label_->setObjectName("settingsLabel");
    language_label_->setMinimumHeight(lang_btn_height);
    {
        int lw = lang_label_width > 0 ? lang_label_width : (language_label_->fontMetrics().horizontalAdvance(language_label_->text()) + 4);
        language_label_->setMinimumWidth(lw);
        language_label_->setMaximumWidth(lw);
    }
    language_label_->setSizePolicy(QSizePolicy::Fixed, language_label_->sizePolicy().verticalPolicy());
    language_label_->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    language_combo_ = new PopupListComboBox;
    language_combo_->setObjectName("langCombo");
    language_combo_->setItemDelegate(new ComboDelegate(language_combo_));
    language_combo_->addItems({"Русский", "English"});
    language_combo_->setFixedSize(lang_btn_width, lang_btn_height);
    language_combo_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(language_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onLanguageChange);
    const int gap = (lang_ == "ru" ? lang_ui::SPACE_LABEL_TO_BTN_GAP_RU : lang_ui::SPACE_LABEL_TO_BTN_GAP_EN)
                  + (lang_ == "ru" ? lang_ui::SPACE_TO_BTN_RU : lang_ui::SPACE_TO_BTN_EN)
                  + (lang_ == "ru" ? lang_ui::LABEL_TO_BTN_OFFSET_RU : lang_ui::LABEL_TO_BTN_OFFSET_EN);
    const int labelShift = (lang_ == "ru") ? lang_ui::LABEL_MOVE_X_RU : lang_ui::LABEL_MOVE_X_EN;
    const int comboShift = (lang_ == "ru") ? lang_ui::BTN_MOVE_X_RU : lang_ui::BTN_MOVE_X_EN;
    const int labelW = language_label_->width();
    const int trackMinW = lang_btn_width + qMax(0, gap) + labelW;
    language_track_ = new LanguageTrackWidget(language_label_, language_combo_);
    language_track_->setAutoFillBackground(false);
    language_track_->setAttribute(Qt::WA_TranslucentBackground);
    language_track_->setFixedHeight(lang_btn_height);
    language_track_->setMinimumWidth(trackMinW);
    language_track_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    static_cast<LanguageTrackWidget*>(language_track_)->setGap(gap);
    static_cast<LanguageTrackWidget*>(language_track_)->setLabelShiftX(labelShift);
    static_cast<LanguageTrackWidget*>(language_track_)->setComboShiftX(comboShift);
    themeRow->addWidget(language_track_, 1, Qt::AlignVCenter);
    geoLog("buildUI", "lang row: gap=" + QString::number(gap) + " labelShift=" + QString::number(labelShift) + " comboShift=" + QString::number(comboShift) + " lang_btn_width=" + QString::number(lang_btn_width));
    btn_right_spacer_ = new QWidget();
    btn_right_spacer_->setAutoFillBackground(false);
    btn_right_spacer_->setAttribute(Qt::WA_TranslucentBackground);
    btn_right_spacer_->setFixedWidth(std::max(0, lang_ui::SPACE_BTN_RIGHT));
    themeRow->addWidget(btn_right_spacer_, 0, Qt::AlignVCenter);
    sl->addLayout(themeRow);

    auto* pathBlock = new QFrame;
    pathBlock->setObjectName("pathBlock");
    pathBlock->setMinimumHeight(180);
    auto* pl = new QVBoxLayout(pathBlock);
    pl->setContentsMargins(6, 10, 12, 12);
    data_files_label_ = new QLabel(trKey("data_files"));
    data_files_label_->setStyleSheet("font-weight: 600; font-size: 15px; background: transparent; border: none;");
    pl->addWidget(data_files_label_);
    geosite_entry_ = new QLineEdit;
    geosite_entry_->setPlaceholderText(trKey("path_geosite"));
    pl->addWidget(geosite_entry_);
    open_geosite_btn_ = new QPushButton(trKey("open_geosite"));
    open_geosite_btn_->setObjectName("secondary");
    open_geosite_btn_->setFlat(true);
    connect(open_geosite_btn_, &QPushButton::clicked, this, &MainWindow::onBrowseGeoSite);
    pl->addWidget(open_geosite_btn_);
    geoip_entry_ = new QLineEdit;
    geoip_entry_->setPlaceholderText(trKey("path_geoip"));
    pl->addWidget(geoip_entry_);
    open_geoip_btn_ = new QPushButton(trKey("open_geoip"));
    open_geoip_btn_->setObjectName("secondary");
    open_geoip_btn_->setFlat(true);
    connect(open_geoip_btn_, &QPushButton::clicked, this, &MainWindow::onBrowseGeoIP);
    pl->addWidget(open_geoip_btn_);
    sl->addWidget(pathBlock);

    auto* statsBlock = new QFrame;
    statsBlock->setObjectName("statsBlock");
    statsBlock->setMinimumHeight(200);
    auto* stl = new QVBoxLayout(statsBlock);
    stl->setContentsMargins(12, 10, 12, 12);
    stl->setSpacing(6);
    stats_label_ = new QLabel(trKey("stats"));
    stats_label_->setStyleSheet("font-weight: 600; font-size: 15px; background: transparent; border: none;");
    stl->addWidget(stats_label_);
    auto* sc1 = new QFrame;
    sc1->setObjectName("statCard");
    auto* sc1l = new QVBoxLayout(sc1);
    sc1l->setContentsMargins(10, 8, 10, 8);
    stat_cat_label_ = new QLabel(trKey("tab_categories"));
    stat_cat_label_->setObjectName("statTitle");
    sc1l->addWidget(stat_cat_label_);
    stat_categories_ = new QLabel("0");
    stat_categories_->setObjectName("statValue");
    sc1l->addWidget(stat_categories_);
    stl->addWidget(sc1);
    auto* sc2 = new QFrame;
    sc2->setObjectName("statCard");
    auto* sc2l = new QVBoxLayout(sc2);
    sc2l->setContentsMargins(10, 8, 10, 8);
    stat_dom_label_ = new QLabel(trKey("tab_domains"));
    stat_dom_label_->setObjectName("statTitle");
    sc2l->addWidget(stat_dom_label_);
    stat_domains_ = new QLabel("0");
    stat_domains_->setObjectName("statValue");
    sc2l->addWidget(stat_domains_);
    stl->addWidget(sc2);
    auto* sc3 = new QFrame;
    sc3->setObjectName("statCard");
    auto* sc3l = new QVBoxLayout(sc3);
    sc3l->setContentsMargins(10, 8, 10, 8);
    stat_ip_label_ = new QLabel("IP");
    stat_ip_label_->setObjectName("statTitle");
    sc3l->addWidget(stat_ip_label_);
    stat_ips_ = new QLabel("0");
    stat_ips_->setObjectName("statValue");
    sc3l->addWidget(stat_ips_);
    stl->addWidget(sc3);
    sl->addWidget(statsBlock);

    auto* utilLayout = new QHBoxLayout;
    copy_btn_ = new QPushButton(trKey("copy"));
    copy_btn_->setObjectName("secondary");
    copy_btn_->setFlat(true);
    connect(copy_btn_, &QPushButton::clicked, this, &MainWindow::onCopyResult);
    utilLayout->addWidget(copy_btn_);
    save_btn_ = new QPushButton(trKey("save"));
    save_btn_->setObjectName("secondary");
    save_btn_->setFlat(true);
    connect(save_btn_, &QPushButton::clicked, this, &MainWindow::onSaveResult);
    utilLayout->addWidget(save_btn_);
    sl->addLayout(utilLayout);
    sl->addStretch();

    mainLayout->addWidget(sidebar);

    auto* mainArea = new QFrame;
    mainArea->setObjectName("mainArea");
    auto* mal = new QVBoxLayout(mainArea);
    mal->setContentsMargins(18, 14, 18, 16);
    mal->setSpacing(10);

    search_title_label_ = new QLabel(trKey("search"));
    search_title_label_->setStyleSheet("font-size: 22px; font-weight: 600; background: transparent; border: none;");
    mal->addWidget(search_title_label_);

    status_label_ = new QLabel;
    status_label_->setObjectName("statusLabel");
    progress_ = new QProgressBar;
    progress_->setMaximum(0);
    progress_->setFixedWidth(220);
    progress_->setVisible(false);
    auto* statusRow = new QHBoxLayout;
    statusRow->addWidget(status_label_);
    statusRow->addStretch();
    statusRow->addWidget(progress_);
    mal->addLayout(statusRow);

    auto* queryCard = new QFrame;
    queryCard->setObjectName("queryCard");
    queryCard->setMinimumHeight(180);
    auto* ql = new QGridLayout(queryCard);
    ql->setContentsMargins(12, 12, 12, 12);
    domain_label_ = new QLabel(trKey("domain"));
    ql->addWidget(domain_label_, 0, 0);
    domain_entry_ = new QLineEdit;
    domain_entry_->setPlaceholderText("youtube.com");
    ql->addWidget(domain_entry_, 0, 1);
    find_cat_btn_ = new QPushButton(trKey("find_categories"));
    find_cat_btn_->setObjectName("primary");
    find_cat_btn_->setFlat(true);
    find_cat_btn_->setMinimumWidth(200);
    connect(find_cat_btn_, &QPushButton::clicked, this, &MainWindow::onSearchDomain);
    ql->addWidget(find_cat_btn_, 0, 2);

    geosite_tag_label_ = new QLabel("geosite:tag");
    ql->addWidget(geosite_tag_label_, 1, 0);
    geosite_tag_entry_ = new QLineEdit;
    geosite_tag_entry_->setPlaceholderText("geosite:youtube");
    ql->addWidget(geosite_tag_entry_, 1, 1);
    auto* geoBtns = new QHBoxLayout;
    domains_btn_ = new QPushButton(trKey("domains"));
    domains_btn_->setObjectName("secondary");
    domains_btn_->setFlat(true);
    domains_btn_->setFixedWidth(95);
    connect(domains_btn_, &QPushButton::clicked, this, &MainWindow::onGetDomains);
    geoBtns->addWidget(domains_btn_);
    dns_btn_ = new QPushButton("IP (DNS)");
    dns_btn_->setObjectName("secondary");
    dns_btn_->setFlat(true);
    dns_btn_->setFixedWidth(95);
    connect(dns_btn_, &QPushButton::clicked, this, &MainWindow::onGetIPsFromDNS);
    geoBtns->addWidget(dns_btn_);
    ql->addLayout(geoBtns, 1, 2);

    geoip_tag_label_ = new QLabel("geoip:tag");
    ql->addWidget(geoip_tag_label_, 2, 0);
    geoip_tag_entry_ = new QLineEdit;
    geoip_tag_entry_->setPlaceholderText("geoip:ru");
    ql->addWidget(geoip_tag_entry_, 2, 1);
    geoip_btn_ = new QPushButton(trKey("ip_from_geoip"));
    geoip_btn_->setObjectName("primary");
    geoip_btn_->setFlat(true);
    geoip_btn_->setMinimumWidth(200);
    connect(geoip_btn_, &QPushButton::clicked, this, &MainWindow::onGetIPsFromGeoIP);
    ql->addWidget(geoip_btn_, 2, 2);
    ql->setColumnStretch(1, 1);
    mal->addWidget(queryCard);

    auto* resultsCard = new QFrame;
    resultsCard->setObjectName("resultsCard");
    resultsCard->setMinimumHeight(300);
    auto* rcl = new QVBoxLayout(resultsCard);
    rcl->setContentsMargins(10, 10, 10, 10);
    results_label_ = new QLabel(trKey("results"));
    results_label_->setStyleSheet("font-weight: 600; font-size: 15px; background: transparent; border: none;");
    rcl->addWidget(results_label_);

    auto* tabRow = new QHBoxLayout;
    tab_cat_btn_ = new QPushButton(trKey("tab_categories"));
    tab_cat_btn_->setObjectName("tabActive");
    tab_cat_btn_->setFlat(true);
    connect(tab_cat_btn_, &QPushButton::clicked, this, &MainWindow::onTabCategories);
    tab_dom_btn_ = new QPushButton(trKey("tab_domains"));
    tab_dom_btn_->setObjectName("tabInactive");
    tab_dom_btn_->setFlat(true);
    connect(tab_dom_btn_, &QPushButton::clicked, this, &MainWindow::onTabDomains);
    tab_ip_btn_ = new QPushButton(trKey("tab_ip"));
    tab_ip_btn_->setObjectName("tabInactive");
    tab_ip_btn_->setFlat(true);
    connect(tab_ip_btn_, &QPushButton::clicked, this, &MainWindow::onTabIP);
    tabRow->addWidget(tab_cat_btn_);
    tabRow->addWidget(tab_dom_btn_);
    tabRow->addWidget(tab_ip_btn_);
    rcl->addLayout(tabRow);

    results_stack_ = new QStackedWidget;
    result_categories_ = new QTextEdit;
    result_categories_->setReadOnly(true);
    result_domains_ = new QTextEdit;
    result_domains_->setReadOnly(true);
    result_ips_ = new QTextEdit;
    result_ips_->setReadOnly(true);
    results_stack_->addWidget(result_categories_);
    results_stack_->addWidget(result_domains_);
    results_stack_->addWidget(result_ips_);
    rcl->addWidget(results_stack_, 1);
    mal->addWidget(resultsCard, 1);

    mainLayout->addWidget(mainArea, 1);

    connect(domain_entry_, &QLineEdit::returnPressed, this, &MainWindow::onSearchDomain);
    connect(geosite_tag_entry_, &QLineEdit::returnPressed, this, &MainWindow::onGetDomains);
    connect(geoip_tag_entry_, &QLineEdit::returnPressed, this, &MainWindow::onGetIPsFromGeoIP);
}

void MainWindow::onThemeChange(int) {
    QString v = theme_combo_->currentData().toString();
    if (v == "system") {
        updateThemeFromSystem();
        return;
    }
    removeSystemThemeListener();
    bool dark = (v == "dark");
    applyTheme(dark);
}

void MainWindow::onLanguageChange(int) {
    lang_ = (language_combo_->currentText() == "Русский") ? "ru" : "en";
    geoLog("onLanguageChange", "lang_=" + lang_ + " comboText=" + language_combo_->currentText());
    const int w = (lang_ == "ru") ? lang_ui::BTN_WIDTH_RU : lang_ui::BTN_WIDTH_EN;
    language_combo_->setFixedSize(w, lang_ui::BTN_HEIGHT);
    btn_right_spacer_->setFixedWidth(std::max(0, lang_ui::SPACE_BTN_RIGHT));
    retranslateUi();
    refreshSettingsRowStyles();
}

void MainWindow::retranslateUi() {
    theme_label_->setText(trKey("theme"));
    QString themeKey = theme_combo_->currentData().toString();
    if (themeKey.isEmpty()) themeKey = "system";
    theme_combo_->blockSignals(true);
    theme_combo_->clear();
    theme_combo_->addItem(trKey("theme_dark"), "dark");
    theme_combo_->addItem(trKey("theme_white"), "white");
    theme_combo_->addItem(trKey("theme_system"), "system");
    theme_combo_->setCurrentIndex(theme_combo_->findData(themeKey));
    theme_combo_->blockSignals(false);
    const int theme_combo_w = (lang_ == "ru") ? 140 : 115;
    theme_combo_->setFixedSize(theme_combo_w, 40);
    language_label_->setText(trKey("language"));
    {
        int lw = (lang_ == "ru") ? lang_ui::LABEL_WIDTH_RU : lang_ui::LABEL_WIDTH_EN;
        if (lw > 0) { language_label_->setMinimumWidth(lw); language_label_->setMaximumWidth(lw); }
        else { int lw2 = language_label_->fontMetrics().horizontalAdvance(language_label_->text()) + 4; language_label_->setMinimumWidth(lw2); language_label_->setMaximumWidth(lw2); }
    }
    data_files_label_->setText(trKey("data_files"));
    stats_label_->setText(trKey("stats"));
    search_title_label_->setText(trKey("search"));
    results_label_->setText(trKey("results"));
    domain_label_->setText(trKey("domain"));
    stat_cat_label_->setText(trKey("tab_categories"));
    stat_dom_label_->setText(trKey("tab_domains"));
    geosite_entry_->setPlaceholderText(trKey("path_geosite"));
    geoip_entry_->setPlaceholderText(trKey("path_geoip"));
    open_geosite_btn_->setText(trKey("open_geosite"));
    open_geoip_btn_->setText(trKey("open_geoip"));
    copy_btn_->setText(trKey("copy"));
    save_btn_->setText(trKey("save"));
    find_cat_btn_->setText(trKey("find_categories"));
    domains_btn_->setText(trKey("domains"));
    geoip_btn_->setText(trKey("ip_from_geoip"));
    tab_cat_btn_->setText(trKey("tab_categories"));
    tab_dom_btn_->setText(trKey("tab_domains"));
    tab_ip_btn_->setText(trKey("tab_ip"));
}

void MainWindow::onTabCategories() { active_tab_ = 0; results_stack_->setCurrentIndex(0); refreshTabStyles(); current_result_text_ = result_categories_->toPlainText(); }
void MainWindow::onTabDomains()  { active_tab_ = 1; results_stack_->setCurrentIndex(1); refreshTabStyles(); current_result_text_ = result_domains_->toPlainText(); }
void MainWindow::onTabIP()       { active_tab_ = 2; results_stack_->setCurrentIndex(2); refreshTabStyles(); current_result_text_ = result_ips_->toPlainText(); }

void MainWindow::loadDefaultPaths() {
    QDir base(QCoreApplication::applicationDirPath());
    QDir cwd = QDir::current();
    for (const auto& dir : {base, cwd}) {
        if (QFileInfo(dir, "geosite.dat").exists()) { geosite_entry_->setText(dir.absoluteFilePath("geosite.dat")); break; }
    }
    for (const auto& dir : {base, cwd}) {
        if (QFileInfo(dir, "geoip.dat").exists()) { geoip_entry_->setText(dir.absoluteFilePath("geoip.dat")); break; }
    }
}

void MainWindow::applyLanguageRowSizes() {
    if (!language_combo_ || !language_label_ || !language_track_) {
        geoLog("applyLanguageRowSizes", "skip: widget null");
        return;
    }
    const int w = (lang_ == "ru") ? lang_ui::BTN_WIDTH_RU : lang_ui::BTN_WIDTH_EN;
    int lw = (lang_ == "ru") ? lang_ui::LABEL_WIDTH_RU : lang_ui::LABEL_WIDTH_EN;
    if (lw <= 0) lw = language_label_->fontMetrics().horizontalAdvance(language_label_->text()) + 4;
    language_label_->setFixedWidth(lw);
    language_label_->setFixedHeight(lang_ui::BTN_HEIGHT);
    language_combo_->setFixedSize(w, lang_ui::BTN_HEIGHT);
    const int gap = (lang_ == "ru" ? lang_ui::SPACE_LABEL_TO_BTN_GAP_RU : lang_ui::SPACE_LABEL_TO_BTN_GAP_EN)
                  + (lang_ == "ru" ? lang_ui::SPACE_TO_BTN_RU : lang_ui::SPACE_TO_BTN_EN)
                  + (lang_ == "ru" ? lang_ui::LABEL_TO_BTN_OFFSET_RU : lang_ui::LABEL_TO_BTN_OFFSET_EN);
    const int labelShift = (lang_ == "ru") ? lang_ui::LABEL_MOVE_X_RU : lang_ui::LABEL_MOVE_X_EN;
    const int comboShift = (lang_ == "ru") ? lang_ui::BTN_MOVE_X_RU : lang_ui::BTN_MOVE_X_EN;
    const int trackMinW = w + qMax(0, gap) + lw;
    language_track_->setMinimumWidth(trackMinW);
    language_track_->setFixedHeight(lang_ui::BTN_HEIGHT);
    static_cast<LanguageTrackWidget*>(language_track_)->setGap(gap);
    static_cast<LanguageTrackWidget*>(language_track_)->setLabelShiftX(labelShift);
    static_cast<LanguageTrackWidget*>(language_track_)->setComboShiftX(comboShift);
    static_cast<LanguageTrackWidget*>(language_track_)->relayout();
    geoLog("applyLanguageRowSizes", "lang_=" + lang_ + " label_text=" + language_label_->text() + " lw=" + QString::number(lw) + " w=" + QString::number(w) + " gap=" + QString::number(gap) + " shift=" + QString::number(labelShift) + " comboShift=" + QString::number(comboShift) + " trackMinW=" + QString::number(trackMinW));
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    geoLog("showEvent", "language_row_fix_applied_=" + QString::number(language_row_fix_applied_));
    // Recalc on first show after styles/layout are applied; otherwise dark theme/Russian breaks spacing.
    if (!language_row_fix_applied_) {
        language_row_fix_applied_ = true;
        geoLog("showEvent", "scheduling onLanguageChange(0) via singleShot");
        QTimer::singleShot(0, this, [this]() { onLanguageChange(0); });
    }
}

void MainWindow::applyLanguage() {
    geoLog("applyLanguage", "lang_=" + lang_);
    language_combo_->blockSignals(true);
    language_combo_->setCurrentIndex(lang_ == "ru" ? 0 : 1);
    language_combo_->blockSignals(false);
    const int w = (lang_ == "ru") ? lang_ui::BTN_WIDTH_RU : lang_ui::BTN_WIDTH_EN;
    language_combo_->setFixedSize(w, lang_ui::BTN_HEIGHT);
    btn_right_spacer_->setFixedWidth(std::max(0, lang_ui::SPACE_BTN_RIGHT));
    retranslateUi();
    applyLanguageRowSizes();
}

void MainWindow::setStatus(const QString& text, bool busy) {
    status_label_->setText(text);
    progress_->setVisible(busy);
}

void MainWindow::setResultText(const QString& text, int tabIndex) {
    current_result_text_ = text;
    active_tab_ = tabIndex;
    if (tabIndex == 0) { result_categories_->setPlainText(text); results_stack_->setCurrentIndex(0); }
    else if (tabIndex == 1) { result_domains_->setPlainText(text); results_stack_->setCurrentIndex(1); }
    else { result_ips_->setPlainText(text); results_stack_->setCurrentIndex(2); }
    refreshTabStyles();
}

void MainWindow::updateStats(int categories, int domains, int ips) {
    if (categories >= 0) stat_categories_->setText(QString::number(categories));
    if (domains >= 0) stat_domains_->setText(QString::number(domains));
    if (ips >= 0) stat_ips_->setText(QString::number(ips));
}

bool MainWindow::ensureGeoSiteLoaded() {
    QString path = geosite_entry_->text().trimmed();
    if (path.isEmpty()) { showMessage(this, trKey("warning"), trKey("set_geosite_path"), QMessageBox::Warning); return false; }
    if (!QFileInfo::exists(path)) { showMessage(this, trKey("error"), trKey("file_not_found").arg(path), QMessageBox::Critical); return false; }
    if (loaded_geosite_path_ == path && geosite_data_) return true;
    setStatus(trKey("loading_geosite"), true);
    QCoreApplication::processEvents();
    geosite_data_ = load_geosite(path.toStdString());
    setStatus(trKey("ready"));
    if (!geosite_data_) { showMessage(this, trKey("error"), trKey("load_geosite_failed").arg(path), QMessageBox::Critical); return false; }
    loaded_geosite_path_ = path;
    return true;
}

bool MainWindow::ensureGeoIPLoaded() {
    QString path = geoip_entry_->text().trimmed();
    if (path.isEmpty()) { showMessage(this, trKey("warning"), trKey("set_geoip_path"), QMessageBox::Warning); return false; }
    if (!QFileInfo::exists(path)) { showMessage(this, trKey("error"), trKey("file_not_found").arg(path), QMessageBox::Critical); return false; }
    if (loaded_geoip_path_ == path && geoip_data_) return true;
    setStatus(trKey("loading_geoip"), true);
    QCoreApplication::processEvents();
    geoip_data_ = load_geoip(path.toStdString());
    setStatus(trKey("ready"));
    if (!geoip_data_) { showMessage(this, trKey("error"), trKey("load_geoip_failed").arg(path), QMessageBox::Critical); return false; }
    loaded_geoip_path_ = path;
    return true;
}

void MainWindow::runBackground(const std::function<std::any()>& work,
                              const std::function<void(std::any, const QString&)>& onDone) {
    QFuture<std::pair<std::any, QString>> future = QtConcurrent::run([work, onDone]() -> std::pair<std::any, QString> {
        try { return {work(), QString()}; } catch (const std::exception& e) { return {std::any(), QString::fromStdString(e.what())}; } catch (...) { return {std::any(), QString("Unknown error")}; }
    });
    auto* watcher = new QFutureWatcher<std::pair<std::any, QString>>(this);
    connect(watcher, &QFutureWatcher<std::pair<std::any, QString>>::finished, this, [watcher, onDone, this]() {
        auto p = watcher->future().result();
        onDone(p.first, p.second);
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

void MainWindow::onBrowseGeoSite() {
    QString path = QFileDialog::getOpenFileName(this, trKey("choose_geosite"), QString(), "V2Ray data (*.dat);;" + trKey("all_files") + " (*.*)");
    if (!path.isEmpty()) geosite_entry_->setText(path);
}

void MainWindow::onBrowseGeoIP() {
    QString path = QFileDialog::getOpenFileName(this, trKey("choose_geoip"), QString(), "V2Ray data (*.dat);;" + trKey("all_files") + " (*.*)");
    if (!path.isEmpty()) geoip_entry_->setText(path);
}

void MainWindow::onSearchDomain() {
    QString domain = domain_entry_->text().trimmed();
    if (domain.isEmpty()) { showMessage(this, trKey("info"), trKey("enter_domain"), QMessageBox::Information); return; }
    if (!ensureGeoSiteLoaded()) return;
    setStatus(trKey("searching_categories"));
    auto cats = search_domain_in_geosite(geosite_data_.get(), domain.toStdString());
    QString output;
    for (const auto& c : cats) output += QString::fromStdString("geosite: " + c + "\n");
    if (output.isEmpty()) output = trKey("not_found");
    setResultText(output, 0);
    updateStats(static_cast<int>(cats.size()), -1, -1);
    setStatus(trKey("categories_found").arg(cats.size()));
}

void MainWindow::onGetDomains() {
    QString tag = geosite_tag_entry_->text().trimmed();
    if (tag.isEmpty()) { showMessage(this, trKey("info"), trKey("enter_geosite_tag"), QMessageBox::Information); return; }
    if (!ensureGeoSiteLoaded()) return;
    setStatus(trKey("getting_domains"));
    auto domains = get_domains_from_geosite(geosite_data_.get(), tag.toStdString());
    QString body;
    size_t limit = 1200;
    for (size_t i = 0; i < std::min(domains.size(), limit); ++i) body += QString::fromStdString(domains[i] + "\n");
    if (domains.size() > limit) body += "\n" + trKey("and_more").arg(domains.size() - limit);
    QString output = trKey("domains_count").arg(domains.size()) + "\n\n" + (body.isEmpty() ? trKey("empty_list") : body);
    setResultText(output, 1);
    updateStats(-1, static_cast<int>(domains.size()), -1);
    setStatus(trKey("domains_received").arg(domains.size()));
}

void MainWindow::onGetIPsFromDNS() {
    QString tag = geosite_tag_entry_->text().trimmed();
    if (tag.isEmpty()) { showMessage(this, trKey("info"), trKey("enter_geosite_tag"), QMessageBox::Information); return; }
    if (!ensureGeoSiteLoaded()) return;
    setResultText(trKey("dns_starting"), 2);
    setStatus(trKey("dns_running"), true);
    runBackground(
        [this, tag]() -> std::any {
            auto domains = get_domains_from_geosite(geosite_data_.get(), tag.toStdString());
            std::vector<std::string> resolvable;
            for (const auto& d : domains) {
                if (d.find("keyword:") == 0 || d.find("regexp:") == 0) continue;
                std::string clean = d;
                if (clean.find("full:") == 0) clean = clean.substr(5);
                if (clean.find('.') != std::string::npos && clean[0] != '.') resolvable.push_back(clean);
            }
            if (resolvable.size() > 300) resolvable.resize(300);
            std::set<std::string> ips;
            std::vector<std::future<std::vector<std::string>>> futures;
            for (const auto& dom : resolvable) futures.push_back(std::async(std::launch::async, [dom]() { return resolve_domain_to_ip(dom, 2.0); }));
            for (auto& f : futures) for (const auto& ip : f.get()) ips.insert(ip);
            std::vector<std::string> sorted(ips.begin(), ips.end());
            std::sort(sorted.begin(), sorted.end());
            return DnsResult{static_cast<int>(resolvable.size()), sorted};
        },
        [this](std::any result, const QString& err) {
            setStatus(QString(), false);
            if (!err.isEmpty()) { setResultText(trKey("error") + ": " + err, 2); setStatus(trKey("dns_error")); return; }
            try {
                auto r = std::any_cast<DnsResult>(result);
                QString output = QString("Резолвинг по geosite: проверено доменов %1\nIP адресов: %2\n\n").arg(r.checked).arg(r.ips.size());
                for (const auto& ip : r.ips) output += QString::fromStdString(ip + "\n");
                if (r.ips.empty()) output += trKey("ips_not_found");
                setResultText(output, 2);
                updateStats(-1, -1, static_cast<int>(r.ips.size()));
                setStatus(trKey("dns_done").arg(r.ips.size()));
            } catch (...) { setStatus(trKey("dns_error")); }
        });
}

void MainWindow::onGetIPsFromGeoIP() {
    QString tag = geoip_tag_entry_->text().trimmed();
    if (tag.isEmpty()) { showMessage(this, trKey("info"), trKey("enter_geoip_tag"), QMessageBox::Information); return; }
    if (!ensureGeoIPLoaded()) return;
    setStatus(trKey("getting_geoip_ranges"));
    auto ips = get_ips_from_geoip(geoip_data_.get(), tag.toStdString());
    QString body;
    size_t limit = 1200;
    for (size_t i = 0; i < std::min(ips.size(), limit); ++i) body += QString::fromStdString(ips[i] + "\n");
    if (ips.size() > limit) body += "\n" + trKey("and_more").arg(ips.size() - limit);
    QString output = trKey("ip_cidr_count").arg(ips.size()) + "\n\n" + (body.isEmpty() ? trKey("empty_list") : body);
    setResultText(output, 2);
    updateStats(-1, -1, static_cast<int>(ips.size()));
    setStatus(trKey("ranges_received").arg(ips.size()));
}

void MainWindow::onCopyResult() {
    if (current_result_text_.isEmpty()) { showMessage(this, trKey("info"), trKey("no_results_to_copy"), QMessageBox::Information); return; }
    QApplication::clipboard()->setText(current_result_text_);
    setStatus(trKey("copied"));
}

void MainWindow::onSaveResult() {
    if (current_result_text_.isEmpty()) { showMessage(this, trKey("info"), trKey("no_results_to_save"), QMessageBox::Information); return; }
    QString path = QFileDialog::getSaveFileName(this, trKey("save_result"), QString(), "Text (*.txt);;All (*.*)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) { f.write(current_result_text_.toUtf8()); setStatus(trKey("saved_to").arg(path)); }
}

} // namespace geochecker
