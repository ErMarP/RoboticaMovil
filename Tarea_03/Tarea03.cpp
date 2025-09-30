#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"

using namespace std::chrono_literals;

class SonarRvizNode : public rclcpp::Node 
{
public:
  SonarRvizNode() : Node("sonar_rviz_node") 
  {
    // Crear un publicador para el tópico de marcadores
    _marker_publisher = this->create_publisher<visualization_msgs::msg::Marker>("sonar_marker", 10);
    
    // Configurar el puerto serial
    configurarPuertoSerial();

    // Crear un temporizador que llame a la función 'publicarMarcador' cada 500ms
    _timer = this->create_wall_timer(500ms, std::bind(&SonarRvizNode::publicarMarcador, this));
  }

  // Destructor para cerrar el puerto serial al terminar
  ~SonarRvizNode() {
    if (_serial_port >= 0) {
      close(_serial_port);
    }
  }

private:
  // ----- Variables de ROS -----
  rclcpp::TimerBase::SharedPtr _timer;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr _marker_publisher;

  // ----- Variables para el puerto serial -----
  int _serial_port = -1;
  const char* _dispositivo_serial = "/dev/ttyACM0"; // ¡Verifica este puerto!
  int _velocidad = B9600;

  void configurarPuertoSerial() {
    // Abrir el archivo del dispositivo serial
    _serial_port = open(_dispositivo_serial, O_RDWR | O_NOCTTY);
    if (_serial_port < 0) {
      RCLCPP_ERROR(this->get_logger(), "Error al abrir el puerto serial: %s", _dispositivo_serial);
      return;
    }

    // Estructura para configurar los atributos del terminal
    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(_serial_port, &tty) != 0) {
        RCLCPP_ERROR(this->get_logger(), "Error al obtener atributos del terminal.");
        return;
    }

    // Configurar velocidad de entrada y salida (baudios)
    cfsetospeed(&tty, _velocidad);
    cfsetispeed(&tty, _velocidad);

    // Configuración de la comunicación
    tty.c_cflag |= (CLOCAL | CREAD); // Habilitar receptor y modo local
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       // 8 bits de datos
    tty.c_cflag &= ~PARENB;   // Sin paridad
    tty.c_cflag &= ~CSTOPB;   // 1 bit de parada
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    // Configurar tiempos de espera para la lectura
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; // Espera hasta 1 segundo por datos

    // Aplicar la nueva configuración
    if (tcsetattr(_serial_port, TCSANOW, &tty) != 0) {
        RCLCPP_ERROR(this->get_logger(), "Error al establecer atributos del terminal.");
        return;
    }

    RCLCPP_INFO(this->get_logger(), "Puerto serial configurado exitosamente en %s", _dispositivo_serial);
  }

  float leerDistanciaSonar() {
    if (_serial_port < 0) return -1.0f;

    char read_buf[256];
    memset(&read_buf, '\0', sizeof(read_buf));
    
    // Leer datos del puerto serial
    int num_bytes = read(_serial_port, &read_buf, sizeof(read_buf) - 1);

    if (num_bytes > 0) {
      std::string dato_str(read_buf);
      std::stringstream ss(dato_str);
      float distancia;
      ss >> distancia;
      if (!ss.fail()) {
        return distancia;
      }
    }
    return -1.0f; // Retorna un valor inválido si la lectura falla
  }

  void publicarMarcador() {
    float distancia_cm = leerDistanciaSonar();
    if (distancia_cm < 0.0f) {
      RCLCPP_WARN(this->get_logger(), "No se recibieron datos válidos del sonar.");
      return;
    }

    RCLCPP_INFO(this->get_logger(), "Distancia recibida: %.2f cm", distancia_cm);

    auto marker = visualization_msgs::msg::Marker();
    marker.header.frame_id = "base_link"; // Marco de referencia del robot
    marker.header.stamp = this->get_clock()->now();

    marker.ns = "sonar";
    marker.id = 0;
    marker.type = visualization_msgs::msg::Marker::SPHERE;
    marker.action = visualization_msgs::msg::Marker::ADD;

    marker.pose.position.x = distancia_cm / 100.0f;
    marker.pose.position.y = 0.0;
    marker.pose.position.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Una esfera de 20cm de diámetro
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;

    // Lógica de color: cambia según la distancia
    if (distancia_cm < 20.0) { // Rojo si está muy cerca
        marker.color.r = 1.0f;
        marker.color.g = 0.0f;
        marker.color.b = 0.0f;
    } else if (distancia_cm < 100.0) { // Amarillo si está a media distancia
        marker.color.r = 1.0f;
        marker.color.g = 1.0f;
        marker.color.b = 0.0f;
    } else { // Verde si está lejos
        marker.color.r = 0.0f;
        marker.color.g = 1.0f;
        marker.color.b = 0.0f;
    }
    marker.color.a = 0.8;

    marker.lifetime = rclcpp::Duration::from_seconds(1.0);

    _marker_publisher->publish(marker);
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SonarRvizNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}