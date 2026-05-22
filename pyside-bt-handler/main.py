import sys

from PySide6.QtWidgets import QApplication

from main_window import MainWindow

__version__ = "v0.1.0"


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Irrigator BT Handler")
    app.setStyle("Fusion")

    window = MainWindow()
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
