import matplotlib.pyplot as plt

#Read the data from the file
charFile_path = 'charCount.csv' 
wordFile_path = 'wordCount.csv'  

characters = []
words = []
frequencies = []
frequencies2 = []

with open(charFile_path, 'r', encoding='utf-8', errors='replace') as file:
    # Skip the header
    next(file)
    
    for line in file:
        # Split by the colon to extract character and frequency
        char, freq = line.strip().split(',')
        char = char.strip()
        # remove replacement character
        if '�' in char or char == '':
                continue
        
        characters.append(char)  # Remove any surrounding spaces
        frequencies.append(int(freq.strip()))  # Convert frequency to an integer


sorted_char_data = sorted(zip(characters, frequencies), key=lambda x: x[1], reverse= True)


with open(wordFile_path, 'r', encoding='utf-8', errors='replace') as file:
    # Skip the header
    next(file)
    
    for line in file:
        # Split by the colon to extract character and frequency
        string, freq = line.strip().split(',')
        words.append(string.strip())  # Remove any surrounding spaces
        frequencies2.append(int(freq.strip()))  # Convert frequency to an integer


sorted_word_data = sorted(zip(words, frequencies2), key=lambda x: x[1], reverse= True)


top_n = 20
characters_sorted, frequencies_sorted = zip(*sorted_char_data)
words_sorted, frequencies2_sorted = zip(*sorted_word_data[:top_n])

fig,(ax1,ax2) = plt.subplots(1,2, figsize=(12,6))

ax1.bar(characters_sorted, frequencies_sorted)
ax1.set_title('Character Frequency Histogram')
ax1.set_xlabel('Character')
ax1.set_ylabel('Frequency')


ax2.bar(words_sorted, frequencies2_sorted)
ax2.set_title('Word Frequency Histogram')
ax2.set_xlabel('Word')
ax2.set_ylabel('Frequency')
ax2.set_xticklabels(words_sorted,rotation=90)


# Step 3: Show the plot
plt.tight_layout()
plt.savefig('histograms.png', format='png')  # Save as PNG file
plt.show()