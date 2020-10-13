import React from 'react';
import DeviceDetails from '../Components/DeviceDetails';

class Devices extends React.Component {
    render() {
        const {devices} = this.props;
        return (
            <div className='devices'>
                <h1>Devices</h1>
                {
                    devices.map(dev => <DeviceDetails setDeviceState={this.props.setDeviceState} key={dev.id} {...dev} />)
                }
            </div>
        );
    }
};

export default Devices;