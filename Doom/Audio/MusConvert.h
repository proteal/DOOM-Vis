//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class MusConvert
{
public:
	MusConvert(const shared_ptr<const vector<ubyte>>& data, vector<ubyte>& outBuffer)
	{
		assert(data);
		const ubyte* bytes = data->data();

		outBuffer.resize(MAX_MIDI_LEN);
		unsigned char* out = outBuffer.data();

		const array<ubyte, AudioDefs::MIDI_MAP_SIZE>& midiMap = AudioDefs::MIDI_MAP();

		// mus header and instruments
		MUSheader_t header;

		// current position in read buffer
		const unsigned char* cur = bytes;
		const unsigned char* end;

		// Midi header(format 0)
		MidiHeaderChunk_t midiHeader;
		// Midi track header, only 1 needed(format 0)
		MidiTrackChunk_t midiTrackHeader;
		// Stores the position of the midi track header(to change the size)
		byte* midiTrackHeaderOut;

		// Delta time for midi event
		int delta_time = 0;
		int temp;
		int channel_volume[AudioDefs::MIDI_MAXCHANNELS] = { 0 };
		int bytes_written = 0;
		int channelMap[AudioDefs::MIDI_MAXCHANNELS], currentChannel = 0;

		// read the mus header
		memcpy(&header, cur, sizeof(header));
		cur += sizeof(header);

		// only 15 supported
		if (header.channels > AudioDefs::MIDI_MAXCHANNELS - 1)
		{
			THROW("Too many MUS channels.")
		}

		// Map channel 15 to 9(percussions)
		for (temp = 0; temp < AudioDefs::MIDI_MAXCHANNELS; ++temp)
		{
			channelMap[temp] = -1;
			channel_volume[temp] = 0x40;
		}
		channelMap[15] = 9;

		// Get current position, and end of position
		cur = bytes + header.scoreStart;
		end = cur + header.scoreLen;

		// Write out midi header
		Midi_CreateHeader(&midiHeader, 0, 1, 0x0059);
		Midi_UpdateBytesWritten(&bytes_written, AudioDefs::MIDIHEADERSIZE, MAX_MIDI_LEN);
		memcpy(out, &midiHeader, AudioDefs::MIDIHEADERSIZE);        // cannot use sizeof(packs it to 16 bytes)
		out += AudioDefs::MIDIHEADERSIZE;

		// Store this position, for later filling in the midiTrackHeader
		Midi_UpdateBytesWritten(&bytes_written, sizeof(midiTrackHeader), MAX_MIDI_LEN);
		midiTrackHeaderOut = out;
		out += sizeof(midiTrackHeader);

		// microseconds per quarter note(yikes)
		Midi_UpdateBytesWritten(&bytes_written, 7, MAX_MIDI_LEN);
		out = Midi_WriteTempo(out, 0x001aa309);

		// Percussions channel starts out at full volume
		Midi_UpdateBytesWritten(&bytes_written, 4, MAX_MIDI_LEN);
		out = WriteByte(out, 0x00);
		out = WriteByte(out, 0xB9);
		out = WriteByte(out, 0x07);
		out = WriteByte(out, 127);

		// Main Loop
		while (cur < end) {
			byte channel;
			byte event;
			byte temp_buffer[32];        // temp buffer for current iterator
			byte *out_local = temp_buffer;
			byte status, bit1, bit2 = 0, bitc = 2;

			// Read in current bit
			event = *cur++;
			channel = (event & 15);                // current channel

			// Write variable length delta time
			out_local += WriteVarLen(delta_time, out_local);

			if (channelMap[channel] < 0)
			{
				// Set all channels to 127 volume
				out_local = WriteByte(out_local, static_cast<byte>(0xB0 + currentChannel));
				out_local = WriteByte(out_local, 0x07);
				out_local = WriteByte(out_local, 127);
				out_local = WriteByte(out_local, 0x00);

				channelMap[channel] = currentChannel++;
				if (currentChannel == 9)
					++currentChannel;
			}

			status = static_cast<byte>(channelMap[channel]);

			// Handle ::g->events
			MusEvent musEvent = static_cast<MusEvent>((event & 122) >> 4);
			switch (musEvent)
			{
			default:
				THROW("Unknown MUS event.");
				break;

			case MusEvent::KEY_OFF:
				status |= 0x80;
				bit1 = *cur++;
				bit2 = 0x40;
				break;

			case MusEvent::KEY_ON:
				status |= 0x90;
				bit1 = *cur & 127;
				if (*cur++ & 128)        // volume bit?
					channel_volume[channelMap[channel]] = *cur++;
				bit2 = static_cast<byte>(channel_volume[channelMap[channel]]);
				break;

			case MusEvent::PITCH_WHEEL:
				status |= 0xE0;
				bit1 = (*cur & 1) >> 6;
				bit2 = (*cur++ >> 1) & 127;
				break;

			case MusEvent::CHANNEL_MODE:
				status |= 0xB0;
				assert(*cur < sizeof(midiMap) / sizeof(midiMap[0]));
				bit1 = midiMap[*cur++];
				bit2 = (*cur++ == 12) ? static_cast<byte>(header.channels + 1) : 0x00;
				break;

			case MusEvent::CONTROLLER_CHANGE:
				if (*cur == 0) {
					cur++;
					status |= 0xC0;
					bit1 = *cur++;
					bitc = 1;
				}
				else {
					status |= 0xB0;
					assert(*cur < sizeof(midiMap) / sizeof(midiMap[0]));
					bit1 = midiMap[*cur++];
					bit2 = *cur++;
				}
				break;

			case MusEvent::UNKNOWN5:        // Unknown
				THROW("Unknown MUS event 5.");
				break;

			case MusEvent::END:        // End
				status = 0xff;
				bit1 = 0x2f;
				bit2 = 0x00;
				assert(cur == end);
				break;

			case MusEvent::UNKNOWN7:        // Unknown
				THROW("Unknown MUS event 7.");
				break;
			}

			// Write it out
			out_local = WriteByte(out_local, status);
			out_local = WriteByte(out_local, bit1);
			if (bitc == 2)
				out_local = WriteByte(out_local, bit2);

			// Write out temp stuff
			if (out_local != temp_buffer)
			{
				Midi_UpdateBytesWritten(&bytes_written, out_local - temp_buffer, MAX_MIDI_LEN);
				memcpy(out, temp_buffer, out_local - temp_buffer);
				out += out_local - temp_buffer;
			}

			if (event & 128)
			{
				delta_time = 0;
				do {
					delta_time = delta_time * 128 + (*cur & 127);
				} while ((*cur++ & 128));
			}
			else
			{
				delta_time = 0;
			}
		}

		// Write out track header
		WriteInt(midiTrackHeader.name, 'MTrk');
		WriteInt(&midiTrackHeader.length, out - midiTrackHeaderOut - sizeof(midiTrackHeader));
		memcpy(midiTrackHeaderOut, &midiTrackHeader, sizeof(midiTrackHeader));

		// Store length written
		outBuffer.resize(bytes_written);
	}

private:
	static const uint MAX_MIDI_LEN = 1024 * 1024;

	// Format - 0(1 track only), 1(1 or more tracks, each play same time), 2(1 or more, each play seperatly)
	void Midi_CreateHeader(MidiHeaderChunk_t* header, short format, short track_count, short division)
	{
		WriteInt(header->name, 'MThd');
		WriteInt(&header->length, 6);
		WriteShort(&header->format, format);
		WriteShort(&header->ntracks, track_count);
		WriteShort(&header->division, division);
	}

	int Midi_UpdateBytesWritten(int* bytes_written, int to_add, int max)
	{
		*bytes_written += to_add;
		if (max && *bytes_written > max)
		{
			THROW("Exceeded max size MIDI.");
		}
		return 1;
	}

	unsigned char* Midi_WriteTempo(unsigned char* buffer, int tempo)
	{
		buffer = WriteByte(buffer, 0x00);        // delta time
		buffer = WriteByte(buffer, 0xff);        // sys command
		buffer = WriteShort(buffer, 0x5103); // command - set tempo

		buffer = WriteByte(buffer, tempo & 0x000000ff);
		buffer = WriteByte(buffer, (tempo & 0x0000ff00) >> 8);
		buffer = WriteByte(buffer, static_cast<byte>((tempo & 0x00ff0000) >> 16));

		return buffer;
	}

	// writes a byte, and returns the buffer
	unsigned char* WriteByte(void* buf, byte b)
	{
		unsigned char* buffer = (unsigned char*)buf;
		*buffer++ = b;
		return buffer;
	}

	unsigned char* WriteShort(void* b, unsigned short s)
	{
		unsigned char* buffer = (unsigned char*)b;
		*buffer++ = (s >> 8);
		*buffer++ = (s & 0x00FF);
		return buffer;
	}

	unsigned char* WriteInt(void* b, unsigned int i)
	{
		unsigned char* buffer = (unsigned char*)b;
		*buffer++ = (i & 0xff000000) >> 24;
		*buffer++ = static_cast<unsigned char>((i & 0x00ff0000) >> 16);
		*buffer++ = (i & 0x0000ff00) >> 8;
		*buffer++ = (i & 0x000000ff);
		return buffer;
	}

	// Writes a variable length integer to a buffer, and returns bytes written
	int WriteVarLen(long value, byte* out)
	{
		long buffer, count = 0;

		buffer = value & 0x7f;
		while ((value >>= 7) > 0) {
			buffer <<= 8;
			buffer += 0x80;
			buffer += (value & 0x7f);
		}

		for (;;)
		{
			++count;
			*out = (byte)buffer;
			++out;
			if (buffer & 0x80)
				buffer >>= 8;
			else
				break;
		}
		return count;
	}
};
