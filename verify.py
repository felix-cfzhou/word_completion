
class wordCompletion():
    def __init__(self):
        self.freq = {}
        self.ids = {}
        self.words = []

    def access(self, w):
        if w not in self.freq:
            self.ids[w] = len(self.ids)
            self.words.append(w)
        self.freq[w] = self.freq.get(w, 0) + 1
        return self.ids[w]

    def getCompletions(self, w, k):
        result = []
        words = sorted(self.freq.keys(), key = lambda x: self.freq[x], reverse=True)
        for i in range(0, len(w)):
            sub = w[:i]
            words = [word for word in words if word.startswith(sub)]
            subresult = [self.ids[word] for word in words[:k]]
            subresult += [-1] * (k - len(subresult))
            result.append(subresult)
        return result

    def verifyCompletions(self, w, k, completions):
        model_completions = self.getCompletions(w, k) # Used only to compare frequencies
        if len(completions) != len(w) + 1:
            raise Exception("Wrong length")
        for model_list, given_list in zip(model_completions, completions):
            num_negative_ones = model_list.count(-1)
            if given_list.count(-1) != num_negative_ones:
                print(model_completions)
                raise Exception("Wrong number of -1's, expected " + str(num_negative_ones))
            if num_negative_ones != 0 and set(given_list[-num_negative_ones:]) != {-1}:
                raise Exception("-1's not at end")
            model_freq = [(self.freq[self.words[id]] if id != -1 else -100) for id in model_list]
            given_freq = [(self.freq[self.words[id]] if id != -1 else -100) for id in given_list]
            if model_freq != given_freq:
                raise Exception("Frequencies don't match")
            actual_ids = [x for x in given_list if x != -1]
            if len(set(actual_ids)) != k - num_negative_ones:
                raise Exception("Repeated id's")
            if sorted(given_freq, reverse=True) != given_freq:
                raise Exception("Freq's are not sorted")

def main():
    w = wordCompletion()
    count = 0
    while True:
        token, k = input().split()
        if token == "[END]":
            print(k, "nanoseconds per word")
            break
        k = int(k)
        id = int(input())
        sz1, sz2 = map(int, input().split())
        completions = []
        for i in range(sz1):
            completions.append(list(map(int, input().split())))
            '''
            if len(completions[-1]) != sz2:
                raise Exception("Bad length")
            '''
        try:
            w.verifyCompletions(token, k, completions)
        except Exception as e:
            print(e)
            print(token, k, completions)
            print(w.freq, w.ids, w.words)
            raise e
        model_id = w.access(token)
        if id != model_id:
            raise Exception("Bad result of access, expected " + model_id)
        count += 1
        print(token, k, sz1, sz2, len(completions[-1]), count)
    print("PASSED!")

if __name__ == "__main__":
    main()
