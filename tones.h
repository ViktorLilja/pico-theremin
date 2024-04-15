#pragma once

/* Helper functions for calculating tones. 
 * Tones are indexed by n, where the frequency of tone is is 440Hz * 2^(n/12).
 * The index can be negative.
*/

// Returns the frequency of a given tone
double frequency_of_tone(int n) {
  return 440.0 * pow(2, 0.083333333 * n);
}

// Get closest tone to frequency.  
int closest_tone(double frequency) {
  //          f = 440Hz * 2^(n/12)
  //      f/440 = 2^(n/12)
  // log(f/440) = (n/12)*log(2)
  //          n = 12 * log(f/440) / log(2)
  //          n = (12/log(2)) * (log(f) - log(440))
  
  return round(17.3123404907 * (log(frequency) - 6.08677472691));
}


// Returns a string representing the name of a tone
void name_of_tone(char *name_buffer, int n) {

  char tone_labels[12][3] = {"A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#"};

  int octave = 4 + (n + 10) / 12;
  if (n <= -10) octave--;
  
  int label_number = n % 12;
  if (label_number < 0) label_number+= 12;

  sprintf(name_buffer, "%s%d\0", tone_labels[label_number], octave);
}
