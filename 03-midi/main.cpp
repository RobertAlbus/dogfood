#include "Clover/IO/MidiIn.h"
#include "Clover/IO/MidiV2.h"

#include <cstdlib>
#include <iostream>

using namespace Clover;;

// int main(int argc, char *argv[]) {
//     IO::MidiIn::printPorts();

// }



// int main(int argc, char *argv[])
// try
// {
//   libremidi::observer obs{{}, libremidi::midi2::observer_default_configuration()};
//   libremidi::midi_out midiout{{}, libremidi::midi2::out_default_configuration()};

//   libremidi::midi_in midiin{
//       {
//           // Set our callback function.
//           .on_message
//           = [&](const libremidi::ump& message) {
//     std::cout << message << std::endl;
//     if (midiout.is_port_connected())
//       midiout.send_ump(message);
//           }
//       },
//       libremidi::midi2::in_default_configuration()};

//   auto pi = obs.get_input_ports();
//   if (pi.empty())
//   {
//     std::cerr << "No MIDI 2 device found\n";
//     return -1;
//   }

//   for (const auto& port : pi)
//   {
//     std::cout << "In: " << port.port_name << " | " << port.display_name << std::endl;
//   }

//   auto po = obs.get_output_ports();
//   for (const auto& port : po)
//   {
//     std::cout << "Out: " << port.port_name << " | " << port.display_name << std::endl;
//   }
//   midiin.open_port(pi[0]);

//   if(!po.empty())
//     midiout.open_port(po[0]);
//   std::cout << "\nReading MIDI input ... press <enter> to quit.\n";

//   char input;
//   std::cin.get(input);
// }
// catch (const std::exception& error)
// {
//   std::cerr << error.what() << std::endl;
//   return EXIT_FAILURE;
// }





int main(int argc, char *argv[])
try
{
  libremidi::observer obs{{}, libremidi::midi2::observer_default_configuration()};
  libremidi::midi_out midiout{{}, libremidi::midi2::out_default_configuration()};

  libremidi::midi_in midiin {
      {
          .on_message
            = [&](const libremidi::ump& message) {
              // std::cout << message << std::endl;

              cmidi2_ump* b = (cmidi2_ump*) &message;
              int bytes = cmidi2_ump_get_num_bytes(message.bytes[0]);
              int group = cmidi2_ump_get_group(b);
              int status = cmidi2_ump_get_status_code(b);
              int channel = cmidi2_ump_get_channel(b);

              int ch;
              uint value;
              float value_f;
              switch ((libremidi::message_type)status)
              {
                case libremidi::message_type::NOTE_ON:
                  ch = cmidi2_ump_get_midi2_note_note(b);
                  value = (uint) cmidi2_ump_get_midi2_note_velocity(b);
                  value_f = value * (1.0f / UINT16_MAX);
                  break;
                case libremidi::message_type::NOTE_OFF:
                  ch = cmidi2_ump_get_midi2_note_note(b);
                  value = cmidi2_ump_get_midi2_note_velocity(b);
                  value_f = value * (1.0f / UINT16_MAX);
                  break;
                case libremidi::message_type::CONTROL_CHANGE:
                  ch = cmidi2_ump_get_midi2_cc_index(b);
                  value = (uint) cmidi2_ump_get_midi2_cc_data(b);
                  value_f = value * (1.0f / UINT_MAX);
                  break;

                default:
                  break;
              }

              // printf("[ bytes %i | group %i | ch %u | value_f %f | value %u ]\n", bytes, group, ch, value_f, value);
              // printf("%d - %u \n", value == 0, value);
              printf("%u \n", value);
          }
      },
      libremidi::midi2::in_default_configuration()
    };

  auto pi = obs.get_input_ports();
  if (pi.empty())
  {
    std::cerr << "No MIDI 2 device found\n";
    return -1;
  }

  for (const auto& port : pi)
  {
    std::cout << "In: " << port.port_name << " | " << port.display_name << std::endl;
  }

  auto po = obs.get_output_ports();
  for (const auto& port : po)
  {
    std::cout << "Out: " << port.port_name << " | " << port.display_name << std::endl;
  }
  midiin.open_port(pi[0]);

  if(!po.empty())
    midiout.open_port(po[0]);
  std::cout << "\nReading MIDI input ... press <enter> to quit.\n";

  char input;
  std::cin.get(input);
}
catch (const std::exception& error)
{
  std::cerr << error.what() << std::endl;
  return EXIT_FAILURE;
}
