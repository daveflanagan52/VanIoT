import React from 'react';
import { Chart } from 'react-charts';
import Card from '../../Components/Card';

import './Environment.scss';

class Environment extends React.Component {
    render() {
        const { temperatures } = this.props;
        const data = [
            {
                label: 'Outside',
                data: temperatures.map(x => [x[2], x[1]]),
            },
            {
                label: 'Inside',
                data: temperatures.map(x => [x[2], x[0]]),
            }
        ];
        const axes = [
            { primary: true, type: 'linear', position: 'bottom' },
            { type: 'linear', position: 'left' }
        ];

        return (
            <div className='environment'>
                <h1>Environment</h1>
                <Card className='p-3'>
                    <div className='env-chart'>
                        <Chart data={data} axes={axes} />
                    </div>
                </Card>
            </div>
        );
    }
};

export default Environment;