// -----------------------------------------------------------------------------------
// Locale English (default,) ISO ISO639-1 language code "en"
#pragma once

// General
#define L_UPLOAD "Cargar"
#define L_DOWNLOAD "Descargar"
#define L_ACCEPT "Aceptar"
#define L_CONTINUE "Continuar"
#define L_ARE_YOU_SURE "Está seguro?"
#define L_LOGOUT "Cerrar sesión"

#define L_UT "UT"
#define L_LST "LST"
#define L_LONG "Longitud"
#define L_LAT "Latitud"
#define L_EAST "Este"
#define L_WEST "Oeste"
#define L_ZENITH "Zenith"
#define L_NCP "NCP"
#define L_SCP "SCP"

#define L_NONE "Ninguno"
#define L_AUTO "Automatico"
#define L_SET_LABEL "Desplazarse a"
#define L_OK "Ok"
#define L_ON "On"
#define L_OFF "Off"
#define L_START "Iniciar"
#define L_DELTA "Delta"
#define L_ZERO "Zero"
#define L_SPAN "Span"
#define L_POWER "Pwr"
#define L_GUIDE "Guiar"
#define L_CENTER "Centrar"
#define L_BLUE "Azul"
#define L_GREEN "Verde"
#define L_RATE "Velocidad"
#define L_RANGE "Rango"
#define L_UNKNOWN "Desconocido"
#define L_FAULT "Fallo"
#define L_POINT "Punto"
#define L_COMPLETE "Completo"
#define L_ACTIVE "Activado"
#define L_INACTIVE "Desactivado"

// ---------------------------- Index ----------------------------
// date/time
#define L_TIME_BROWSER_UT "UT (web browser)"

// weather
#define L_TEMPERATURE "Temperatura"
#define L_AMBIENT_TEMPERATURE "Temperatura Ambiente"
#define L_PRESSURE "Presión Barométrica"
#define L_HUMIDITY "Humedad Relativa"
#define L_DEW_POINT "Temperatura Punto de Rocío"

// operation
#define L_TRACKING "Seguimiento"
#define L_SITE "Ubicación"
#define L_COORDINATES "Coordenadas"
#define L_CURRENT "Posición"
#define L_TARGET "Objetivo&nbsp;&nbsp;"
#define L_ENCODER "Encoder"
#define L_POLAR_ALIGN "Alineamiento Polar"

// pier side
#define L_PIER_SIDE "Pier Side"
#define L_MERIDIAN_FLIPS "cambios de meridiano"
#define L_ALIGN_MESSAGE "Montura relativa a"
#define L_MERIDIAN_FLIP_W_TO_E "Cambio de meridiano, Oeste a Este"
#define L_MERIDIAN_FLIP_E_TO_W "Cambio de meridiano, Este a Oeste"

#define L_OPERATIONS "Operaciones"

// parking
#define L_PARKING "Estacionando"
#define L_PARKED "Estacionado"
#define L_NOT_PARKED "No estacionado"
#define L_PARKING "Estacionando"
#define L_PARK_FAILED "Estacionamiento falló"

// home
#define L_HOMING "Yendo a Posición de Inicio"
#define L_AT_HOME "En Posición de Inicio"
#define L_SLEWING "Girando"
#define L_PPS_SYNC "Sincronizar PPS"

// tracking
#define L_REFR_COMP_RA "RC"
#define L_REFR_COMP_BOTH "RCD"
#define L_FULL_COMP_RA "FC"
#define L_FULL_COMP_BOTH "FCD"
#define L_TRACKING "Seguimiento"
#define L_CURRENT_MAXRATE "Velocidad Max. de seguimiento"
#define L_DEFAULT_MAXRATE "Velocidad máxima predeterminada"
#define L_CURRENT_MAXSLEW "Velocidad de giro máxima"

#define L_FREQUENCY_ADJUST "Ajuste de Frecuencia"

// driver status
#define L_DRIVER "Driver"
#define L_STATUS "Estado"
#define L_STATE "Estado"
#define L_COMMS_FAILURE "Error de comunicación"
#define L_STANDSTILL "Parado"
#define L_OPEN_LOAD "Carga abierta"
#define L_SHORT_GND "Corto a Tierra"
#define L_SHUTDOWN_OVER "Apagar por completo"
#define L_PRE_WARNING "Pre-Advertencia"
#define L_DRIVER_FAULT "Falla"

// misc. errors/workload
#define L_INTERNAL_TEMP "Temperatura interna del controlador"
#define L_LAST_GENERAL_ERROR "Último error"
#define L_WORKLOAD "Carga de trabajo"
#define L_WIRELESS_SIGNAL_STRENGTH "Intensidad de la señal inalámbrica"

// --------------------------- Control ---------------------------

#if DISPLAY_SPECIAL_CHARS == ON
  #define HOME_CH "&#x1F3E0;"
  #define ARROW_DR "&#x27A5;"
  #define ARROW_UR "&#x27A6;"
  #define ARROW_R2 "&#x27A4;"
  #define CAUTION_CH "&#9888;"
  #define CLOCK_CH "&#x1F565;"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "&#9733;"
  #define LUNAR_CH "&#9790;"
  #define SOLAR_CH "&#9737;"
  #define KING_CH "&#9812;"
#else
  #define HOME_CH "H"
  #define ARROW_DR "-&gt;"
  #define ARROW_UR "-&gt;"
  #define ARROW_R2 "&gt;"
  #define CAUTION_CH "!"
  #define CLOCK_CH "T"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "*"
  #define LUNAR_CH "("
  #define SOLAR_CH "O"
  #define KING_CH "K"
#endif

#define BUTTON_N "N"
#define BUTTON_S "S"
#define BUTTON_E "E"
#define BUTTON_W "W"
#define BUTTON_SYNC "@"

#define L_HINT_CLOCK "Establecer fecha/hora"
#define L_HINT_CONTINUE_GOTO "Continuar GoTo (cuando está en pausa en posición de Inicio)"
#define L_HOME_PARK_TITLE "Localización y Posición de Inicio"
#define L_FIND_HOME "Buscar Posición de Inicio"
#define L_RESET_HOME "Posición de Inicio (Resetear)"
#define L_UNPARK "Desestacionar"
#define L_PARK "Estacionar"
#define L_HINT_EMERGENCY_STOP "Parada de Emergencia!"
#define L_HINT_RATE_SIDEREAL "Velocidad Sideral"
#define L_HINT_RATE_LUNAR "Velocidad Lunar"
#define L_HINT_RATE_SOLAR "Velocidad Solar"
#define L_HINT_RATE_KING "Velocidad King"
#define L_HINT_FOC_SEL "Selecionar Enfocador"
#define L_HINT_DEROTATE_ON "DesRotador Encendido"
#define L_HINT_ROTATOR_REV "Invertir Movimiento Rotador"
#define L_HINT_ROTATOR_PAR "Mover el Rotador al ángulo  Paraláctico "
#define L_HINT_DEROTATE_OFF "Des-Rotador Apagado"

#define L_ALIGN "Alinear"
#define L_AUX_FEATURES "Características Auxiliares"

#define L_FOCUSER "Enfocador"
#define L_ROTATOR "Rotador"
#define L_DEROTATOR "Des-Rotador"
#define L_CAMERA_EXPOSURE "Exposición"
#define L_CAMERA_DELAY "Retraso"
#define L_CAMERA_COUNT "Cuenta"

// ---------------------------- Library --------------------------

#define L_CATALOG "Catálogo"
#define L_CATALOGS "Catálogos"
#define L_RECS_AVAIL "Registros Disponibles"
#define L_CAT_DATA "Datos (descargados O para cargar)"

#define L_CAT_EXAMPLE1 "Campos:<br /><pre>"
#define L_CAT_EXAMPLE2 "  Se requiere un registro de nombre de catálogo para la fila 1, este<br />"
#define L_CAT_EXAMPLE3 "   consiste en un '$' seguido de hasta 10 caracteres.<br /><br />"
#define L_CAT_EXAMPLE4 "   El nombre del objeto es una secuencia de hasta 11 caracteres.<br /><br />"
#define L_CAT_EXAMPLE5 "  Cat. es un código de categoría de la siguiente manera:<br />"
#define L_CAT_EXAMPLE6 "  RA (Jnow) está en horas, minutos y segundos.<br />"
#define L_CAT_EXAMPLE7 "  Dec (Jnow) está en +/- Grados, Minutos, Segundos.<br />"

#define L_CAT_NO_NAME "Catálogo seleccionado, no tiene nombre."
#define L_CAT_GET_NAME_FAIL "Catálogo seleccionado, error al obtener el nombre."
#define L_CAT_NO_CAT "Ningún catálogo seleccionado."
#define L_CAT_NO_OBJECT "Objeto no seleccionado."
#define L_SELECTED "seleccionado"
#define L_CAT_DOWNLOAD_FAIL "Descarga fallida, datos incorrectos."
#define L_CAT_DOWNLOAD_SUCCESS "Descarga exitosa."
#define L_CAT_DOWNLOAD_INDEX_FAIL "Falló la descarga, no se pudo indexar al catálogo."
#define L_CAT_DATA_REMOVED "Datos del catálogo eliminados"
#define L_CAT_UPLOAD_FAIL "Error en la carga, nombre de catálogo incorrecto."
#define L_CAT_UPLOAD_INDEX_FAIL "Falló la carga, indexando reg. de nombre del catálogo"
#define L_CAT_DELETE_FAIL "Falló la carga, eliminando el reg. de nombre del catálogo"
#define L_CAT_WRITE_NAME_FAIL "Falló la carga, escribiendo el reg. de nombre del catálogo"
#define L_CAT_UPLOAD_NO_NAME_FAIL "Falló la carga, la línea 1 debe contener el nombre del catálogo."
#define L_CAT_BAD_FORM "Carga fallida, formato incorrecto en la línea# "
#define L_CAT_UPLOAD_BAD_OBJECT_NAME "Falló la carga, nombre de objeto incorrecto en la línea# "
#define L_CAT_BAD_CATEGORY "Carga fallida, categoría incorrecta en la línea# "
#define L_CAT_BAD_RA "Falló la carga, formato de RA incorrecto en la línea# "
#define L_CAT_BAD_DEC "Carga fallida, formato de diciembre incorrecto en la línea#  "
#define L_CAT_UPLOAD_RA_FAIL "Carga fallida, enviando RA en línea# "
#define L_CAT_UPLOAD_DEC_FAIL "Falló la carga, enviando Dec en línea# "
#define L_CAT_UPLOAD_LINE_FAIL "Falló la carga, enviando la línea# "
#define L_CAT_UPLOAD_SUCCESS "Carga exitosa"
#define L_CAT_UPLOAD_LINES_WRITTEN "líneas escritas"
#define L_CAT_UPLOAD_SELECT_FAIL "Falló la carga, no se pudo seleccionar el catálogo."
#define L_CAT_UPLOAD_NO_CAT "Falló la carga, no se seleccionó ningún catálogo."
#define L_CAT_CLEAR "Borrar catálogo"
#define L_CAT_CLEAR_LIB "Borrar biblioteca"

// --------------------------- Encoders --------------------------

// general settings
#define L_ENC_SYNC_TO_ENC "Sincronizar OnStep a Encoders"
#define L_ENC_SYNC_TO_ONS "Sincronizar Encoders a OnStep"
#define L_ENC_ORIGIN_TO_ONS "Colocar OnStep -> Encoders en  Origen"
#define L_ENC_AUTO_SYNC "Sincronizar automáticamente   Encoders a OnStep"
#define L_ENC_CONF " Configurar Encoders"
#define L_ENC_SET_TPD "Cuentas por grado"
#define L_ENC_SYNC_THLD "Umbral de Sincronización Automática"
#define L_ENC_SYNC_THLD_UNITS "0 a 9999 arc-sec"
#define L_ENC_AUTO_RATE "Control de velocidad OnStep"
#define L_NOW "Ahora"

// ------------------------------ PEC ----------------------------

#define L_PEC_STATUS "Estado PEC"
#define L_PEC_CONTROL "Control PEC"
#define L_PEC_PLAY "Reproducir"
#define L_STOP "Parar"
#define L_PEC_CLEAR "Borrar"
#define L_PEC_REC "Grabar"
#define L_PEC_CLEAR_MESSAGE "Borrar borra el búfer de la memoria, no la EEPROM. Durante la grabación, las correcciones se promedian 3:1 a favor del búfer a menos que se borre, en cuyo caso se utiliza la corrección completa."
#define L_PEC_EEWRITE "Escribir en EEPROM"
#define L_PEC_EEWRITE_MESSAGE "Escribir los datos PEC en la EEPROM para que se recuerden si OnStep se reinicia. La escritura de los datos puede tardar unos segundos."
#define L_DISABLED_MESSAGE "Deshabilitado"
#define L_PEC_IDLE "Inactivo"
#define L_PEC_WAIT_PLAY "Reproducir esperando para arrancar"
#define L_PEC_PLAYING "Reproduciendo"
#define L_PEC_WAIT_REC "Registro en espera de que llegue el índice"
#define L_PEC_RECORDING "Grabando"
#define L_PEC_UNK "Desconocido"
#define L_PEC_EEWRITING "escribir en EEPROM"

// --------------------------- Settings --------------------------

#define L_REFINE_POLAR_ALIGN "Afinar alineación polar"
#define L_REFINE_PA "Afinar PA"
#define L_REFINE_MESSAGE1 "Configuración y montaje de alineación de más de 3 estrellas. Vaya a una estrella brillante cerca de NCP o SCP con Dec de 50 a 80&deg; rango (N or S.) "
#define L_REFINE_MESSAGE2 "Presione el botón [Afinar PA]. Use los controles de ajuste PA de la montura para centrar la estrella nuevamente."
#define L_REFINE_MESSAGE3 "Opcionalmente, vuelva a alinear la montura."

#define L_PARK "Estacionar"
#define L_SET_PARK "Colocar en Posición de Estacionamiento"
#define L_SET_PARK_CURRENT_COORDS "Establecer la posición de estacionamiento en las coordenadas actuales"

#define L_TRK_FASTER "más rápido"
#define L_TRK_SLOWER "más lento"
#define L_TRK_RESET "Restablecer (predeterminado)"
#define L_TRK_COMP "Frecuencia de seguimiento compensada (modelo de puntería/refracción)"
#define L_TRK_FULL "Completo"
#define L_TRK_REFR "Solo Refracción"
#define L_TRK_DUAL "Doble Eje"
#define L_TRK_SINGLE "Eje Único"

#define L_BUZZER "Alerta de GoTo, Zumbador"

#define L_MERIDIAN_FLIP_AUTO "Volteo de meridiano automático en el límite"
#define L_MERIDIAN_FLIP_NOW "Ahora"
#define L_MERIDIAN_FLIP_PAUSE "Volteo de Meridiano, Pausa en Posicion de Inicio"

// ------------------------ Configuration ------------------------

#define L_BASIC_SET_TITLE "Básico:"

#define L_MAX_SLEW_SPEED "Velocidad de giro máxima"

// goto Rate
#define L_VSLOW "El más Lento"
#define L_SLOW "Lento"
#define L_NORMAL "Normal"
#define L_FAST "Rápido"
#define L_VFAST "El más Rápido"

// backlash
#define L_BACKLASH "Holgura"
#define L_BACKLASH_RANGE_AXIS1 "Axis1, 0 a 3600 arc-sec"
#define L_BACKLASH_RANGE_AXIS2 "Axis2, 0 a 3600 arc-sec"
#define L_BACKLASH_RANGE_AXIS3UP "Retroceso, en pasos 0 a 32767"

// deadband/TCF
#define L_DEADBAND "TCF Deadband"
#define L_TCF_COEF "TCF Coef"
#define L_TCF_COEF_EN "TCF Encendido"
#define L_DEADBAND_RANGE_AXIS4UP "TCF Deadband, 1 a 32767 pasos"
#define L_TCF_COEF_RANGE_AXIS4UP "TCF Coef, &pm;999.0 micro/deg. C"
#define L_TCF_COEF_EN_AXIS4UP "TCF Encendido, 0 falso O 1 verdadero"

// limits
#define L_LIMITS_TITLE "Límites generales y de horizonte"
#define L_LIMITS_RANGE_HORIZON "Horizonte, altitud mínima &pm;30&deg;"
#define L_LIMITS_RANGE_OVERHEAD "Tope Superior, altitud máxima 60 to 90&deg;"
#define L_LIMITS_RANGE_MERIDIAN_E "Meridiano pasado al este  &pm;270&deg;"
#define L_LIMITS_RANGE_MERIDIAN_W "Meridiano pasado al oeste &pm;270&deg;"

// location
#define L_LOCATION_TITLE "Horario y Ubicación"
#define L_LOCATION_LONG "Longitud deg. min. sec. &pm;180, W es +"
#define L_LOCATION_LAT  "Latitud deg. min. sec. &pm;90, N es +"
#define L_LOCATION_RANGE_UTC_OFFSET "Desplazamiento UTC, en horas y min. -14 a +12"
#define L_LOCATION_MESSAGE_UTC_OFFSET "Opuesto del valor de zona horaria, esto es para la hora estándar (no para la hora de verano)."

// advanced configuration
#define L_REVERT "Volver a los valores predeterminados"
#define L_ADV_SET_TITLE "Avanzado"
#define L_ADV_SET_HEADER_MSG "los cambios a continuación surten efecto después de reiniciar OnStep:"
#define L_ADV_MOUNT_TYPE "Seleccione 1 GEM, 2 EQ o 3 para Alt/Azm"
#define L_ADV_BOOL "0 falso O 1 verdadero"

#define L_ADV_SET_SPWR "Pasos por rotación del engranaje"
#define L_ADV_SET_SPD "Pasos por grado"
#define L_ADV_SET_SPM "Pasos por micrón"
#define L_ADV_SET_REV "Invertir dirección"
#define L_ADV_SET_MIN "Posición Mínima"
#define L_ADV_SET_MAX "Posición Máxima"

#define L_ADV_SET_SPECIAL "Step/Dir Driver:<br /><br />Un valor de -1 es equivalente a Config.h OFF. <i>¡Ten cuidado con estos!</i>"
#define L_ADV_SET_IMMEDIATE "Servo Driver:<br /><br />Los ajustes surten efecto inmediatamente, no es necesario reiniciar. <i>¡Ten cuidado con estos!</i>"
#define L_ADV_SET_TRAK_us "Micropasos"
#define L_ADV_SET_GOTO_us "Micropasos en GoTo"
#define L_ADV_SET_HOLD_ma "Corriente en Retención en mA"
#define L_ADV_SET_TRAK_ma "Corriente en Seguimiento en mA"
#define L_ADV_SET_SLEW_ma "Corriente en Giro en mA"
#define L_ADV_SET_P "Proporcional de Seguimiento"
#define L_ADV_SET_I "Integral de Seguimiento"
#define L_ADV_SET_D "Derivativo de Seguimiento"
#define L_ADV_SET_P_GOTO "Proporcional de GoTo"
#define L_ADV_SET_I_GOTO "Integral de GoTo"
#define L_ADV_SET_D_GOTO "Derivativo de GoTo"

#define L_ADV_ENABLE "Activar Configuración Avanzada"
#define L_ADV_DISABLE "Desactivar Configuración Avanzada (Valores por defecto al reiniciar)"
#define L_ADV_SET_AXIS_NO_EDIT "Deshabilitar la edición del eje."
#define L_ADV_SET_NO_EDIT "La edición de todos los ejes está deshabilitada en este momento. Tal vez esta función no esté habilitada (por lo que los valores predeterminados de configuración relacionados están activos) o necesite reiniciar OnStep."
#define L_ADV_SET_FOOTER_MSG1 "Después de presionar [Cargar], tome nota de los cambios anteriores, ya que la carga puede haber sido rechazada debido a un parámetro no válido."
#define L_ADV_SET_FOOTER_MSG2 "Cuando elige [Revertir], ese grupo deja de estar disponible para editar hasta que reinicia OnStep."
#define L_ADV_SET_FOOTER_MSG3 "Si IRUN se establece en un valor diferente al predeterminado de Config.h, IHOLD se establece en(HALF.)"
#define L_ADV_SET_FOOTER_MSG4 "Cambiar la relación de Pasos de Axis1 por rotación de engranajes y Pasos por grado puede dañar el área de memoria de Library NV, primero haga una copia de seguridad de los catálogos si es necesario."

// reset control
#define L_RESET_TITLE "Control de reinicio OnStep:"
#define L_WIPE_RESET "Borrar todas las configuraciones y reiniciar OnStep"
#define L_RESET "Reiniciar OnStep"
#define L_RESET_FWU "Reiniciar OnStep para cargar el firmware"
#define L_RESET_MSG1 "OnStep debe estar en Posicion de Inicio o Estacionado, de lo contrario se ignora la solicitud de reinicio."

// ----------------------------  SETUP ----------------------------

#define L_NETWORK_PERFORMANCE "Rendimiento y compatibilidad"
#define L_NETWORK_CMD_TO "Canal Comando serial lectura time-out"
#define L_NETWORK_WWW_TO "Canal Web serial lectura time-out"

#define L_NETWORK_STA_TITLE "Modo de estación (conectar a un punto de acceso)"
#define L_NETWORK_PWD "Contraseña"
#define L_NETWORK_PWD_MSG "8 caracteres mínimo."
#define L_NETWORK_EN_DHCP "Habilitar DHCP"
#define L_NETWORK_EN_DHCP_MSG "Nota: las direcciones anteriores se ignoran si DHCP está habilitado"
#define L_NETWORK_EN_STA "Habilitar modo de estación"
#define L_IP_ADDRESS "Dirección IP"
#define L_GATEWAY "Puerta de enlace"
#define L_SUBNET "Subred"
#define L_NETWORK_CHA "Canal"
#define L_NETWORK_EN_AP_MODE "Habilitar modo de punto de acceso"

#define L_NETWORK_CONFIG_SECURITY "Configuración de Seguridad de red"

#define L_NETWORK_AP "Modo de punto de acceso"

#define L_NETWORK_RESTART_MSG "Debes <u>manualmente</u> reiniciar para que los cambios surtan efecto."

#define L_NETWORK_TITLE "Ingrese la contraseña para cambiar la configuración:"

#define L_NETWORK_ADVICE1 "Configuración:<br/><br/>"
#define L_NETWORK_ADVICE2 "Habilite el modo Estación <b>O</b> Punto de acceso, habilitar ambos puede causar problemas de rendimiento.<br/><br/>"
#define L_NETWORK_ADVICE3 "Si desea que OnStep se conecte a la red WiFi de su hogar, habilite solo el modo Estación y "
#define L_NETWORK_ADVICE4 "desactivar el modo de punto de acceso. Si OnStep no puede conectarse a la red WiFi de su hogar, lo hará "
#define L_NETWORK_ADVICE5 "automáticamente reactivara el modo Estación; esto es útil cuando está en un sitio oscuro y/o "
#define L_NETWORK_ADVICE6 "para evitar quedar bloqueado.<br/><br/>"
#define L_NETWORK_ADVICE7 "Si no tienes WiFi, puedes usar el IDE de Arduino para restablecer la memoria flash ESP (y  "
#define L_NETWORK_ADVICE8 "por lo tanto, vuelve a los valores predeterminados). En el menú Herramientas de Arduino, cambia Erase Flash 'Only Sketch'"
#define L_NETWORK_ADVICE9 "para borrar Flash 'All Flash Contents' y volver a cargar el boceto WiFi-Bluetooth."

// --------------------------- Constants -------------------------

#define L_DOWN_TITLE "¡La interfaz serial para OnStep está inactiva!"
#define L_DOWN_MESSAGE1 "Si acaba de reiniciar OnStep, espere unos segundos hasta que todo vuelva a iniciarse y luego verifique que todavía esté conectado a Wi-Fi."
#define L_DOWN_MESSAGE2 "Si este es un problema persistente, aquí hay otras posibles causas:<br /><ul>"
#define L_DOWN_MESSAGE3 "Desigualdad en la velocidad de transmisión debido al reinicio de OnStep solamente, apague y encienda todo de nuevo."
#define L_DOWN_MESSAGE4 "Velocidad de baudios SWS configurada incorrectamente."
#define L_DOWN_MESSAGE5 "Velocidad en baudios de OnStep configurada incorrectamente."
#define L_DOWN_MESSAGE6 "Cableado incorrecto de los puertos de comunicación."
#define L_DOWN_MESSAGE7 "Cableado incorrecto de los pines de la fuente de alimentación. Gnd debe compartirse entre la MCU de OnStep y la MCU del Addon."

#define L_CONTROLLER "Controlador" // was Status
#define L_PAGE_MOUNT "Montura" // was Control
#define L_PAGE_AUXILIARY "Auxiliar"
#define L_PAGE_LIBRARY "Biblioteca"
#define L_PAGE_ENCODERS "Encoders"
#define L_PAGE_PEC "PEC"
#define L_SETTINGS "Ajustes"
#define L_CONTROLS "Control"
#define L_PAGE_CONFIG "Configuración"
#define L_PAGE_NETWORK "Red"

// ------------------------- MountStatus -------------------------

// general (background) errors
#define L_GE_NONE "Ninguno"
#define L_GE_MOTOR_FAULT "Fallo de motor/controlador"
#define L_GE_ALT_MIN "Por debajo del límite del horizonte" 
#define L_GE_LIMIT_SENSE "Sensor de Límite"
#define L_GE_DEC "Límite de Dec excedido"
#define L_GE_AZM "Límite de Azm excedido"
#define L_GE_UNDER_POLE "Límite de Polo excedido"
#define L_GE_MERIDIAN "Límite de Meridiano excedido"
#define L_GE_SYNC "Límite de Seguridad de sincronización superado"
#define L_GE_PARK "Estacionamiento fallido"
#define L_GE_GOTO_SYNC "Fallo en sincronización GoTo"
#define L_GE_UNSPECIFIED "Error desconocido"
#define L_GE_ALT_MAX "Por encima del Tope Superior de Alt"
#define L_GE_WEATHER_INIT "Error al iniciar el sensor meteorológico"
#define L_GE_SITE_INIT "Hora o ubicación no actualizada"
#define L_GE_NV_INIT "Inicialización NV/EEPROM mala"
#define L_GE_OTHER "Error desconocido, código"

// command errors
#define L_CE_NONE "Sin errores"
#define L_CE_0 "Respuesta 0"
#define L_CE_CMD_UNKNOWN "comando desconocido"
#define L_CE_REPLY_UNKNOWN "respuesta inválida"
#define L_CE_PARAM_RANGE "parámetro fuera de rango"
#define L_CE_PARAM_FORM "formato de parámetro incorrecto"
#define L_CE_ALIGN_FAIL "falló la alineación"
#define L_CE_ALIGN_NOT_ACTIVE "alineación no está activa"
#define L_CE_NOT_PARKED_OR_AT_HOME "no estacionado o en Posición de Inicio"
#define L_CE_PARKED "ya estacionado"
#define L_CE_PARK_FAILED "estacionamiento fallido"
#define L_CE_NOT_PARKED "no estacionado"
#define L_CE_NO_PARK_POSITION_SET "sin posición de estacionamiento establecida"
#define L_CE_GOTO_FAIL "GoTo falló"
#define L_CE_LIBRARY_FULL "biblioteca llena"
#define L_CE_GOTO_ERR_BELOW_HORIZON "GoTo por debajo del Horizonte"
#define L_CE_GOTO_ERR_ABOVE_OVERHEAD "GoTo por encima del Tope Superior"
#define L_CE_SLEW_ERR_IN_STANDBY "giro en espera"
#define L_CE_SLEW_ERR_IN_PARK "giro en estacionamiento"
#define L_CE_GOTO_ERR_GOTO "ya en GoTo"
#define L_CE_GOTO_ERR_OUTSIDE_LIMITS "GoTo fuera de los límites"
#define L_CE_SLEW_ERR_HARDWARE_FAULT "fallo de hardware"
#define L_CE_MOUNT_IN_MOTION "montura en movimiento"
#define L_CE_GOTO_ERR_UNSPECIFIED "otro"
#define L_CE_UNK "desconocido"
