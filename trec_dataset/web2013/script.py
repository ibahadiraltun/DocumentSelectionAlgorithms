import pandas as pd


df = pd.read_csv('qrels.adhoc13.txt', names = ['qid', 'x1', 'doc', 'rel'], sep = ' ')

df.loc[df['rel'] < 0, 'rel'] = 0
df.loc[df['rel'] > 1, 'rel'] = 1


df.to_csv('qrels_new.txt', index = False, sep = ' ')