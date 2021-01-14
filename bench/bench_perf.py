import subprocess

model_dir = 'models'
bin_path = '../build_release/bin/storm'

amounts = [ int(10**x) for x in range(0, 5) ]
avg_count = 10

models = [
    {
        'file_name': 'beb.3-4.v1.jani',
        'extra_params': ['--constants', 'N=3'],
    },
    {
        'file_name': 'csma.2-2.v1.jani',
        'extra_params': [],
    },
    {
        'file_name': 'zeroconf.v1.prism',
        'extra_params': ['--constants', 'N=20,K=6,reset=true'],
    },
    {
        'file_name': 'test2.prism',
        'extra_params': ['--constants', 'x=0.1'],
    },
    {
        'file_name': 'test3.prism',
        'extra_params': ['--constants', 'x=0.1'],
    },
]

rounds = 2000
for model in models:
    print(model['file_name'])
    for amount in amounts:
        total_time = 0
        total_error = 0
        total_branches = 0
        print(f'runs={amount}, rounds={rounds}:', end=' ', flush=True)
        for _ in range(avg_count):
            params = [bin_path]
            params += model['extra_params']
            params += ['--generateObservations', '--runs', str(amount), '--rounds', str(rounds), '--generateUncertainMdp']

            file_name = model['file_name']
            if file_name.endswith('jani'):
                params.append('--jani')
            elif file_name.endswith('prism'):
                params.append('--prism')
            else:
                print(f'error unknown file format for file "{model.file_name}"', flush=True)
            params.append(f'{model_dir}/{file_name}')

            #print(f'Running: "{params}"')
            result = subprocess.run(params, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
            time_took, error, branches = list(map(float, result.stderr.splitlines()))
            total_time += time_took
            total_error += error
            total_branches += branches
        avg_time = total_time / avg_count
        avg_error = total_error / avg_count
        avg_branches = total_branches / avg_count
        print(f'{avg_time},{avg_error},{avg_branches}', flush=True)
    print(flush=True)
