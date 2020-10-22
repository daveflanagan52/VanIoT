import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faToggleOff, faToggleOn, faLightbulb, faFan, faHeat, faOutlet, faCarBattery, faMapMarked, faLocation, faBurn, faTemperatureLow, faLightSwitch } from '@fortawesome/pro-light-svg-icons';

import './DeviceControl.scss';
import Card from '../Card';

const icons = {
    'lightbulb': faLightbulb,
    'fan': faFan,
    'heat': faHeat,
    'outlet': faOutlet,
    'car-battery': faCarBattery,
    'map-marked': faMapMarked,
    'location': faLocation,
    'burn': faBurn,
    'temperature-low': faTemperatureLow,
    'light-switch': faLightSwitch,
};

export default (props) => {
    return (
        <Card className={'device-control' + (props.state ? ' on' : '')} onClick={() => props.onDeviceClick()}>
            <div className='p-3'>
                <div className=''>
                    <div className='float-right text-right'>
                        <FontAwesomeIcon className='toggle' icon={props.state ? faToggleOn : faToggleOff} />
                        <p className='mb-0'>{props.name}</p>
                    </div>
                    <div className=''><FontAwesomeIcon className='icon fa' icon={icons[props.icon]} /></div>
                </div>
            </div>
        </Card>
    );
};