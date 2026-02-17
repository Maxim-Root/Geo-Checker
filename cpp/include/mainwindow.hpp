#pragma once

#include "dat_parser.hpp"
#include "geochecker_types.hpp"

#include <QMainWindow>
#include <QString>
#include <functional>
#include <any>
#include <memory>

class QLineEdit;
class QTextEdit;
class QLabel;
class QTimer;
class QProgressBar;
class QPushButton;
class QComboBox;
class QStackedWidget;
class QShowEvent;

namespace geochecker {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onThemeChange(int);
    void onLanguageChange(int);
    void onSystemThemeChanged();
    void onBrowseGeoSite();
    void onBrowseGeoIP();
    void onSearchDomain();
    void onTabCategories();
    void onTabDomains();
    void onTabIP();
    void onGetDomains();
    void onGetIPsFromDNS();
    void onGetIPsFromGeoIP();
    void onCopyResult();
    void onSaveResult();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void buildUI();
    void applyLanguageRowSizes();
    void loadDefaultPaths();
    void applyLanguage();
    QString trKey(const char* key) const;
    bool ensureGeoSiteLoaded();
    bool ensureGeoIPLoaded();
    void setStatus(const QString& text, bool busy = false);
    void setResultText(const QString& text, int tabIndex);
    void updateStats(int categories, int domains, int ips);
    void runBackground(const std::function<std::any()>& work,
                      const std::function<void(std::any, const QString&)>& onDone);
    void applyTheme(bool dark);
    void refreshTabStyles();
    /** Refresh fonts/styles of "Theme" and "Language" row and recalc sizes. Call after theme or language change. */
    void refreshSettingsRowStyles();
    void retranslateUi();
    bool isSystemDarkTheme() const;
    void updateThemeFromSystem();
    void setupSystemThemeListener();
    void removeSystemThemeListener();

    std::unique_ptr<GeoSiteData> geosite_data_;
    std::unique_ptr<GeoIPData> geoip_data_;
    QString loaded_geosite_path_;
    QString loaded_geoip_path_;
    QString current_result_text_;
    QString lang_;  // "ru" or "en"
    bool dark_theme_ = true;
    bool language_row_fix_applied_ = false;

    class QComboBox* theme_combo_;
    class QComboBox* language_combo_;
    QPushButton* tab_cat_btn_;
    QPushButton* tab_dom_btn_;
    QPushButton* tab_ip_btn_;
    QStackedWidget* results_stack_;
    QLabel* theme_label_;
    QWidget* lang_spacer_;
    QWidget* language_track_;   // free positioning of "Language" label relative to button
    QWidget* btn_right_spacer_;
    QLabel* language_label_;
    QLabel* data_files_label_;
    QLabel* stats_label_;
    QLabel* search_title_label_;
    QLabel* results_label_;
    QLabel* domain_label_;
    QLabel* geosite_tag_label_;
    QLabel* geoip_tag_label_;
    QLabel* stat_cat_label_;
    QLabel* stat_dom_label_;
    QLabel* stat_ip_label_;
    QPushButton* open_geosite_btn_;
    QPushButton* open_geoip_btn_;
    QPushButton* copy_btn_;
    QPushButton* save_btn_;
    QPushButton* find_cat_btn_;
    QPushButton* domains_btn_;
    QPushButton* dns_btn_;
    QPushButton* geoip_btn_;
    int active_tab_ = 0;

    QLineEdit* geosite_entry_;
    QLineEdit* geoip_entry_;
    QLineEdit* domain_entry_;
    QLineEdit* geosite_tag_entry_;
    QLineEdit* geoip_tag_entry_;

    QLabel* stat_categories_;
    QLabel* stat_domains_;
    QLabel* stat_ips_;
    QLabel* status_label_;

    QProgressBar* progress_;
    QTimer* system_theme_timer_ = nullptr;
    QMetaObject::Connection system_theme_connection_;
    QTextEdit* result_categories_;
    QTextEdit* result_domains_;
    QTextEdit* result_ips_;
};

} // namespace geochecker
