import React from 'react';
import { Map, Marker, TileLayer } from 'react-leaflet';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';

import L from 'leaflet';

import { faThermometerThreeQuarters, faClouds, faCarBattery, faBolt, faFaucetDrip, faSignalStream, faSun, faMoon, faCloudSun, faCloudMoon, faCloud, faCloudShowers, faCloudShowersHeavy, faThunderstorm, faCloudSnow, faFog } from '@fortawesome/pro-light-svg-icons';

import InfoBox from '../../Components/InfoBox';
import Card from '../../Components/Card';
import DeviceControl from '../../Components/DeviceControl';

import './Home.scss';

export const vanIcon = new L.Icon({
    iconUrl: '../../Resources/van.png',
    iconAnchor: [16, 37],
    popupAnchor: [16, -37],
    iconSize: [32, 37],
});

const weatherIcons = {
    '01d': faSun,
    '01n': faMoon,
    '02d': faCloudSun,
    '02n': faCloudMoon,
    '03d': faCloud,
    '03n': faCloud,
    '04d': faClouds,
    '04n': faClouds,
    '09d': faCloudShowers,
    '09n': faCloudShowers,
    '10d': faCloudShowersHeavy,
    '10n': faCloudShowersHeavy,
    '11d': faThunderstorm,
    '11n': faThunderstorm,
    '13d': faCloudSnow,
    '13n': faCloudSnow,
    '50d': faFog,
    '50n': faFog,
};

class Home extends React.Component {
    openNotifications() {
        this.props.notifications.filter(n => !n.read).forEach(n => {
            this.props.onReadNotification(n.id);
        });
        this.setState({showNotifications: true});
    }

    capitalize(s) {
        if (typeof s !== 'string') return ''
        return s.charAt(0).toUpperCase() + s.slice(1)
    }

    render() {
        const { battery, power, water, network, temperatureInside, temperatureOutside, location, weather } = this.props.data;
        const { devices } = this.props;
        
        const weatherDesciption = weather
            ? this.capitalize(weather.current.weather[0].description) + ', ' + weather.daily[0].temp.max + '°C - ' + weather.daily[0].temp.min + '°C'
            : 'Waiting for weather data...';

        const WeatherForecast = weather
            ? weather.daily.slice(1, 5).map((d, i) => {
                let date = new Date(d.dt * 1000);
                return (
                    <div key={i} className='col text-center weather'>
                        <FontAwesomeIcon className='icon' icon={weatherIcons[d.weather[0].icon]} />
                        <p className='desc'>{d.weather[0].main}</p>
                        <p className='temp'>{d.temp.min}&deg;C / {d.temp.max}&deg;C</p>
                        <p className='date'>{date.getDate() + '.' + (date.getMonth() + 1) + '.' + date.getFullYear()}</p>
                    </div>
                )
            })
            : null;

        const favouriteDevices = [
            'front-lights',
            'invertor',
            'kitchen-lights',
            'heater',
            'rear-lights',
            'fan',
        ];
        const deviceList = devices.filter(d => favouriteDevices.includes(d.id)).sort((a, b) => {
            let ai = favouriteDevices.findIndex(e => e === a.id);
            let bi = favouriteDevices.findIndex(e => e === b.id);
            if (ai < bi)
                return -1;
            if (ai > bi)
                return 1;
            return 0;
        }).map((device, index) => {
            return (
                <div className='col-6 mb-3' key={index}>
                    <DeviceControl
                        state={device.state.toggle}
                        name={device.name}
                        icon={device.icon}
                        onDeviceClick={() => {
                            this.props.setDeviceState(device.id, 'toggle', {value:device.state.toggle ? 0 : 1})
                        }} 
                        />
                </div>
            );
        });

        return (
            <div className='home'>
                <h1>Home</h1>                
                <div className='row mb-3 text-center'>
                    <div className='col'>
                        <InfoBox title='Battery' value={battery} unit='%' icon={faCarBattery} />
                    </div>
                    <div className='col'>
                        <InfoBox title='Power' value={power} unit='W' icon={faBolt} />
                    </div>
                    <div className='col'>
                        <InfoBox title='Water' value={water} unit='%' icon={faFaucetDrip} />
                    </div>
                    <div className='col'>
                        <InfoBox title='Network' value={network ? 'Online' : 'Offline'} unit='' icon={faSignalStream} />
                    </div>
                </div>
                <Card className='mb-3'>
                    <div className='row'>
                        <div className='col-5'>                            
                            <div className='p-3'>
                                <p className='lead'>
                                    <FontAwesomeIcon icon={faThermometerThreeQuarters} style={{width:30}} className='mr-2 fa' />
                                    <big>{temperatureOutside}&deg;C</big> outside temperature
                                </p>
                                <p className='lead'>
                                    <FontAwesomeIcon icon={faThermometerThreeQuarters} style={{width:30}} className='mr-2 fa' />
                                    <big>{temperatureInside}&deg;C</big> inside temperature
                                </p>
                                <p className='lead mb-0'>
                                    <FontAwesomeIcon icon={faClouds} style={{width:30}} className='mr-2 fa' />
                                    {weatherDesciption}
                                </p>
                            </div>
                        </div>
                        <div className='col'>
                            <div className='row'>
                                {WeatherForecast}
                            </div>
                        </div>
                    </div>
                </Card>
                <div className='row'>
                    <div className='col-6'>
                        <Card>
                            <Map center={[location.latitude, location.longitude]} zoom={13} className='location-map' attributionControl={false}>
                                <TileLayer url="https://tiles.stadiamaps.com/tiles/alidade_smooth/{z}/{x}/{y}{r}.png" />
                                {/*<TileLayer url="//{s}.tile.osm.org/{z}/{x}/{y}.png" />*/}
                                <Marker position={[location.latitude, location.longitude]} icon={vanIcon} />
                            </Map>
                        </Card>
                    </div>
                    <div className='col-6'>
                        <div className='row'>
                            {deviceList}
                        </div>
                    </div>
                </div>
            </div>
        );
    }
};

export default Home;